#!/usr/bin/env python3
"""Check script for the Newton-iteration assignment.

Pass ``--calibrate`` to measure and report the runtimes instead of checking.

Note that we allow for one more core than the "number of threads" in the
assignment, as we need to accommodate the additional writing thread. The stale
main thread on the other hand does not require designated core allocation, so
the program is told to use ``core_limit - 1`` threads.
"""

from __future__ import annotations

import argparse
import os
import mmap
import re
import subprocess
import sys
from dataclasses import dataclass
from typing import List, Optional

HPC_FOLDER = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if HPC_FOLDER not in sys.path:
  sys.path.append(HPC_FOLDER)

from common.check_submission import (  # noqa: E402
  CheckCase, HyperfineSetup, build_command, run_calibration, run_check)


def verify_ppm_file(
    path: str,
    expected_width: int,
    expected_height: int,
  ) -> Optional[str]:
  """Verify the required plain PPM header and RGB raster in ``path``."""
  pixel_count = expected_width * expected_height
  pattern: re.Pattern[bytes] = re.compile(
    rb"\AP3[ \t]*+\r?\n"
    rb"(?P<width>[0-9]++)[ \t]++(?P<height>[0-9]++)[ \t]*+\r?\n"
    rb"(?P<max_color>[0-9]++)[ \t]*+\r?\n"
    rb"(?:[ \t\r\n\v\f]*+[0-9]++"
       rb"[ \t\r\n\v\f]++[0-9]++"
       rb"[ \t\r\n\v\f]++[0-9]++)"
    rb"{" + str(pixel_count).encode("ascii") + rb"}+" +
    rb"[ \t\r\n\v\f]*+\Z")

  if os.path.getsize(path) == 0:
    return f"Error when checking output: file {path} is empty."

  with (
    open(path, "rb") as ppm_file,
    mmap.mmap(ppm_file.fileno(), 0, access=mmap.ACCESS_READ) as ppm
  ):
    match = pattern.fullmatch(ppm)
    if match is None:
      return (
        f"Error when checking output: file {path} is not an ASCII P3 image "
        f"containing exactly {pixel_count} RGB pixels.")
    width, height, max_color = map(
      int, match.group("width", "height", "max_color"))

  if width != expected_width or height != expected_height:
    return (
      f"Error when checking output: file {path} has dimensions "
      f"{width} x {height}, expected {expected_width} x {expected_height}.")
  if not 1 <= max_color <= 65535:
    return (
      f"Error when checking output: file {path} has invalid maximum color "
      f"value {max_color}; expected a value from 1 to 65535.")

  return None


@dataclass
class CheckCaseNewtonIteration(CheckCase):
  nmb_lines: int
  degree: int
  nmb_threads: int
  picture_path: Optional[str]

  build_exec_name = "newton"

  def __str__(self) -> str:
    return (f"{self.nmb_lines} lines for degree {self.degree} "
            f"on {self.nmb_threads} threads")

  def command(self) -> List[str]:
    return build_command(self.hyperfine_setup, [
      f"./{self.build_exec_name}",
      f"-l{self.nmb_lines}", f"-t{self.nmb_threads}", str(self.degree)])

  def verify(self, stdout_log: str) -> Optional[str]:
    output_roots = [
      f"newton_attractors_x{self.degree}",
      f"newton_convergence_x{self.degree}",
    ]
    for root in output_roots:
      if not os.path.isfile(f"{root}.ppm"):
        return f"Error when checking output: file {root}.ppm does not exist."
      verification_error = verify_ppm_file(
        f"{root}.ppm",
        expected_width=self.nmb_lines,
        expected_height=self.nmb_lines)
      if verification_error is not None:
        return verification_error

    if self.picture_path is not None and not self.hyperfine_setup.is_calibration:
      print("Converting both pictures to png and copying for manual check.")
      os.makedirs(self.picture_path, exist_ok=True)
      for root in output_roots:
        try:
          subprocess.run(
            ["magick", f"{root}.ppm",
             os.path.join(self.picture_path, f"{root}.png")],
            check=True)
        except (OSError, subprocess.CalledProcessError) as error:
          # Picture conversion is only a manual-inspection aid, so a failure
          # here (e.g. magick missing) must not invalidate an otherwise
          # passing run.
          print(
            f"Warning: could not convert {root}.ppm for manual "
            f"inspection: {error}")

    for root in output_roots:
      os.remove(f"{root}.ppm")

    return None


def newton_iteration_case(
    nmb_lines: int,
    degree: int,
    hyperfine: HyperfineSetup,
    picture_path: Optional[str] = None
  ) -> CheckCaseNewtonIteration:
  assert hyperfine.has_core_limit
  assert hyperfine.core_limit > 1
  return CheckCaseNewtonIteration(
    nmb_lines=nmb_lines,
    degree=degree,
    nmb_threads=hyperfine.core_limit - 1,
    picture_path=picture_path,
    hyperfine_setup=hyperfine)


def main(args: List[str]) -> None:
  """Run the Newton-iteration submission check or calibration."""
  parser = argparse.ArgumentParser(
    description="Validate and benchmark a Newton-iteration submission archive.")
  parser.add_argument(
    "submission",
    metavar="SUBMISSION.tar.gz",
    help="path to the submission archive to check")
  parser.add_argument(
    "--calibrate",
    action="store_true",
    help="measure runtimes and suggest limits instead of checking them")
  parsed_args = parser.parse_args(args)

  # Path will only be used after verification of tar.gz suffix in checks.
  pics = os.path.join(
    os.getcwd(), "pictures", os.path.basename(parsed_args.submission)[:-7])

  cases = [
    newton_iteration_case(1000, 1,
      HyperfineSetup(core_limit=2, runtime_limit=0.167, nmb_reps=10, nmb_warmups=5)),
    newton_iteration_case(1000, 2,
      HyperfineSetup(core_limit=2, runtime_limit=0.2, nmb_reps=10, nmb_warmups=2),
      picture_path=pics),
    newton_iteration_case(1000, 5,
      HyperfineSetup(core_limit=2, runtime_limit=0.446, nmb_reps=10, nmb_warmups=2)),
    newton_iteration_case(1000, 7,
      HyperfineSetup(core_limit=2, runtime_limit=0.645, nmb_reps=5, nmb_warmups=1)),

    newton_iteration_case(1000, 5,
      HyperfineSetup(core_limit=3, runtime_limit=0.281, nmb_reps=10, nmb_warmups=2)),
    newton_iteration_case(1000, 5,
      HyperfineSetup(core_limit=4, runtime_limit=0.238, nmb_reps=10, nmb_warmups=2)),
    newton_iteration_case(1000, 5,
      HyperfineSetup(core_limit=5, runtime_limit=0.21, nmb_reps=10, nmb_warmups=5)),

    newton_iteration_case(1000, 7,
      HyperfineSetup(core_limit=11, runtime_limit=0.197, nmb_reps=10, nmb_warmups=2)),
    newton_iteration_case(2000, 7,
      HyperfineSetup(core_limit=11, runtime_limit=0.386, nmb_reps=10, nmb_warmups=2)),
    newton_iteration_case(3000, 7,
      HyperfineSetup(core_limit=11, runtime_limit=0.682, nmb_reps=5, nmb_warmups=1),
      picture_path=pics),
  ]

  submission_args = [parsed_args.submission]
  if parsed_args.calibrate:
    run_calibration(submission_args, cases)
  else:
    run_check(submission_args, cases)
    print(f"Verify pictures in {pics} manually.")
    print("Submission passes the check script.")


if __name__ == "__main__":
  main(sys.argv[1:])
