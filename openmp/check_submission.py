#!/usr/bin/env python3
"""Check script for the cell-distances assignment.

Runs the calibrated cases against the submission and prints the pass marker once
all of them succeed. Pass ``--calibrate`` to instead measure and report the
runtimes (run this against a reference solution to pick the limits below).
"""

from __future__ import annotations

import argparse
import math
import os
import sys
from dataclasses import dataclass
from typing import Dict, List, Optional

HPC_FOLDER = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if HPC_FOLDER not in sys.path:
  sys.path.append(HPC_FOLDER)

from common.check_submission import (  # noqa: E402
  CheckCase, HyperfineSetup, build_command, run_calibration, run_check)

TEST_DATA = os.path.join(HPC_FOLDER, "openmp", "test_data")


@dataclass
class CheckCaseCellDistances(CheckCase):
  cells_path: str
  nmb_threads: int
  reference_paths: Optional[List[str]]

  build_exec_name = "distances"

  def __str__(self) -> str:
    cell_filename = os.path.basename(self.cells_path)
    cell_suffix = cell_filename[cell_filename.rfind("_") + 1:]
    return f"{cell_suffix} with {self.nmb_threads} threads"

  def command(self) -> List[str]:
    symlink_path = "./cells"
    if os.path.lexists(symlink_path):
      os.remove(symlink_path)
    os.symlink(self.cells_path, symlink_path)
    return build_command(
      self.hyperfine_setup,
      [f"./{self.build_exec_name}", f"-t{self.nmb_threads}"])

  def verify(self, stdout_log: str) -> Optional[str]:
    tol_abs = 50
    tol_rel = 0.02
    if self.reference_paths is None:
      return None

    if stdout_log.strip() == "":
      return "Program output is empty"

    try:
      stdout_dist = parse_cell_distances(stdout_log)
    except ValueError as error:
      return _dump_stdout(stdout_log, str(error))

    reference_errors: List[str] = []
    for reference_path in self.reference_paths:
      with open(reference_path) as reference_file:
        reference_dist = parse_cell_distances(reference_file.read())
      error = _compare_to_reference(
        stdout_dist, reference_dist, os.path.basename(reference_path),
        tol_abs, tol_rel)
      if error is None:
        return None
      reference_errors.append(error)

    return _dump_stdout(stdout_log, "\n".join(reference_errors))


def cell_distances_case(
    cells_suffix: str,
    use_reference: bool,
    hyperfine: HyperfineSetup
  ) -> CheckCaseCellDistances:
  assert hyperfine.has_core_limit
  if use_reference:
    hyperfine.requires_stdout = True
    reference_paths: Optional[List[str]] = [
      os.path.join(TEST_DATA, f"dist_{cells_suffix}_trunc"),
      os.path.join(TEST_DATA, f"dist_{cells_suffix}_trunc_shift"),
      os.path.join(TEST_DATA, f"dist_{cells_suffix}_round"),
    ]
  else:
    reference_paths = None
  return CheckCaseCellDistances(
    cells_path=os.path.join(TEST_DATA, f"cells_{cells_suffix}"),
    nmb_threads=hyperfine.core_limit,
    reference_paths=reference_paths,
    hyperfine_setup=hyperfine)


def _dump_stdout(stdout_log: str, error_message: str) -> str:
  dump_path = "distances_stdout"
  with open(dump_path, "w") as dump_file:
    dump_file.write(stdout_log)
  return f"Output of program was written to {dump_path}.\n{error_message}"


def _compare_to_reference(
    stdout_dist: Dict[int, int],
    reference_dist: Dict[int, int],
    reference_name: str,
    tol_abs: int,
    tol_rel: float
  ) -> Optional[str]:
  """Compare a parsed histogram against one reference; return the first error."""
  prefix = (
    f"Error when comparing against reference solution {reference_name}:\n")

  for key in sorted(set(stdout_dist) - set(reference_dist)):
    if stdout_dist[key] > tol_abs:
      return (
        f"{prefix}value at {key / 100} should be zero but is "
        f"{stdout_dist[key]} (applying absolute error tolerance of {tol_abs})")

  for key in sorted(set(reference_dist) - set(stdout_dist)):
    if reference_dist[key] > tol_abs:
      return (
        f"{prefix}value at {key / 100} should be {reference_dist[key]} but is "
        f"zero (applying absolute error tolerance of {tol_abs})")

  for key in sorted(set(stdout_dist) & set(reference_dist)):
    tol = math.floor(max(tol_abs, tol_rel * reference_dist[key]))
    count_min = reference_dist[key] - tol if tol < reference_dist[key] else 0
    count_max = reference_dist[key] + tol
    if stdout_dist[key] < count_min or stdout_dist[key] > count_max:
      return (
        f"{prefix}value at {key / 100} should be between {count_min} and "
        f"{count_max} but is {stdout_dist[key]} "
        f"(applying absolute error tolerance of {tol_abs} and "
        f"relative error tolerance {tol_rel} "
        f"to reference value {reference_dist[key]})")

  return None


def parse_cell_distances(text: str) -> Dict[int, int]:
  """Parse a ``dd.dd <count>`` histogram into ``{scaled_distance: count}``.

  Distances are scaled by 100 (``"12.34"`` becomes ``1234``); zero counts are
  dropped. Raises :class:`ValueError` with a descriptive message on any
  malformed row.
  """
  dist_dict: Dict[int, int] = {}

  if text == "":
    raise ValueError("Error when parsing: string is empty")

  for row_index, row in enumerate(text.split("\n"), start=1):
    if row.strip() == "":
      continue

    parts = row.split(" ")
    if len(parts) != 2:
      raise ValueError(
        f"Error when parsing: {row_index}-th row {row} "
        "cannot be split into exactly two parts")
    distance, count = parts

    if len(distance) != 5:
      raise ValueError(
        f"Error when parsing: {row_index}-th distance {distance} "
        "not of required string length.")
    if distance[2] != ".":
      raise ValueError(
        f"Error when parsing: middle character of {row_index}-th distance "
        f"{distance} is not a point.")
    if not all(distance[i].isdigit() for i in (0, 1, 3, 4)):
      raise ValueError(
        f"Error when parsing: {row_index}-th distance {distance} "
        "does not contain digit where it should.")
    scaled_distance = (int(distance[0]) * 1000 + int(distance[1]) * 100
                       + int(distance[3]) * 10 + int(distance[4]))

    try:
      parsed_count = int(count)
      if parsed_count < 0:
        raise ValueError
    except ValueError:
      raise ValueError(
        f"Error when parsing: {row_index}-th count {count} "
        "is not parsable as unsigned integer.")

    if parsed_count != 0:
      dist_dict[scaled_distance] = parsed_count

  return dist_dict


@dataclass
class CheckCaseCellDistancesMemory(CheckCase):
  cells_path: str
  nmb_threads: int
  memory_limit_bytes: int
  runtime_allowance_bytes: int

  build_exec_name = "distances"

  massif_output = "massif.out"


  def __str__(self) -> str:
    cell_filename = os.path.basename(self.cells_path)
    cell_suffix = cell_filename[cell_filename.rfind("_") + 1:]
    return f"heap memory for {cell_suffix} with {self.nmb_threads} threads"

  def command(self) -> List[str]:

    symlink_path = "./cells"
    if os.path.lexists(symlink_path):
      os.remove(symlink_path)
    os.symlink(self.cells_path, symlink_path)

    if os.path.exists(self.massif_output):
      os.remove(self.massif_output)

    # We terminate the program as opposed to killing it to distinguish error
    # codes.
    massif_timeout_seconds = 5
    massif_kill_after_seconds = 10
    return [
      "timeout", "-s", "TERM", "-k", str(massif_kill_after_seconds),
      str(massif_timeout_seconds),
      "valgrind", "--tool=massif", f"--massif-out-file={self.massif_output}",
      f"./{self.build_exec_name}", f"-t{self.nmb_threads}",
    ]

  def expected_return_codes(self) -> List[int]:
    # A full instrumented run of the largest case is prohibitively slow. The
    # allocations happen while reading the input, before the quadratic work,
    # so timeout's status is the expected result of a successful profile.
    return [0, 124]

  def verify(self, stdout_log: str) -> Optional[str]:
    del stdout_log
    try:
      peak_bytes = parse_massif_peak(self.massif_output)
    except (OSError, ValueError) as error:
      return f"Could not read Massif output {self.massif_output}: {error}"

    measured_limit_bytes = (
      self.memory_limit_bytes + self.runtime_allowance_bytes)
    if peak_bytes > measured_limit_bytes:
      return (
        f"Peak heap allocation was {peak_bytes} bytes, exceeding the "
        f"limit of {self.memory_limit_bytes} bytes plus the "
        f"{self.runtime_allowance_bytes}-byte runtime allowance.")
    return None


def cell_distances_memory_case(
    cells_suffix: str,
    nmb_threads: int,
    memory_limit_bytes: int,
    runtime_allowance_bytes: int
  ) -> CheckCaseCellDistancesMemory:
  return CheckCaseCellDistancesMemory(
    cells_path=os.path.join(TEST_DATA, f"cells_{cells_suffix}"),
    nmb_threads=nmb_threads,
    memory_limit_bytes=memory_limit_bytes,
    runtime_allowance_bytes=runtime_allowance_bytes,
    hyperfine_setup=HyperfineSetup())


def parse_massif_peak(path: str) -> int:
  """Return the largest useful-heap measurement in a Massif output file."""
  peak_bytes: Optional[int] = None
  with open(path) as massif_file:
    for line in massif_file:
      if not line.startswith("mem_heap_B="):
        continue
      try:
        snapshot_bytes = int(line.removeprefix("mem_heap_B=").strip())
      except ValueError:
        raise ValueError(f"invalid mem_heap_B line: {line.rstrip()}")
      peak_bytes = max(peak_bytes or 0, snapshot_bytes)

  if peak_bytes is None:
    raise ValueError("no mem_heap_B snapshots found")
  return peak_bytes


def main(args: List[str]) -> None:
  """Run the cell-distances submission check or calibration."""
  parser = argparse.ArgumentParser(
    description="Validate and benchmark a cell_distances submission archive.")
  parser.add_argument(
    "submission",
    metavar="SUBMISSION.tar.gz",
    help="path to the submission archive to check")
  parser.add_argument(
    "--calibrate",
    action="store_true",
    help="measure runtimes and suggest limits instead of checking them")
  parsed_args = parser.parse_args(args)

  cases = [
    cell_distances_case("1e4", True,
      HyperfineSetup(core_limit=1, runtime_limit=0.346, nmb_reps=10, nmb_warmups=1)),
    cell_distances_case("1e5", True,
      HyperfineSetup(core_limit=5, runtime_limit=6.71, nmb_reps=2, nmb_warmups=1)),
    cell_distances_case("1e5", True,
      HyperfineSetup(core_limit=10, runtime_limit=3.7, nmb_reps=2, nmb_warmups=1)),
    cell_distances_case("1e5", True,
      HyperfineSetup(core_limit=20, runtime_limit=1.99, nmb_reps=4, nmb_warmups=1)),
  ]
  if not parsed_args.calibrate:
    cases.insert(0, cell_distances_memory_case("1e5",
      nmb_threads=20, memory_limit_bytes=1024 * 1024, runtime_allowance_bytes=32 * 1024))

  submission_args = [parsed_args.submission]
  if parsed_args.calibrate:
    run_calibration(submission_args, cases)
  else:
    run_check(submission_args, cases)
    print("Submission passes the check script.")


if __name__ == "__main__":
  main(sys.argv[1:])
