#! /usr/bin/python3

"""Generate a file of random points, one point per line.

Each line holds `-c` space-separated coordinates. Every coordinate is a
random multiple of 10^(-d) in the range [-b, b], written with a sign and
padded to a fixed width so that all coordinates line up in columns.
"""

import argparse
import random


def create_point_line(nmb_coords: int, bound: int, decimals: int) -> str:
  """Return one line with `nmb_coords` random, fixed-width coordinates."""
  scale = 10 ** decimals
  # Width = sign + integer digits + '.' + decimal digits.
  width = 1 + len(str(bound)) + 1 + decimals
  coordinate_format = f"{{:+0{width}.{decimals}f}}"
  coordinates = (
    random.randint(-bound * scale, bound * scale) / scale
    for _ in range(nmb_coords)
  )
  return " ".join(
    coordinate_format.format(coordinate) for coordinate in coordinates
  ) + "\n"


def main() -> None:
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument(
    '-c', type=int, default=3, help='number of coordinates per point')
  parser.add_argument(
    '-d', type=int, default=3, help='number of decimal places')
  parser.add_argument(
    '-b', type=int, default=10, help='coordinate bound')
  parser.add_argument(
    '-n', type=int, required=True, help='number of points')
  parser.add_argument('-s', type=int, default=None, help='random seed')
  parser.add_argument(
    'output_name', metavar='output', type=str, help='output file name')
  args = parser.parse_args()

  if args.s is not None:
    random.seed(args.s)

  with open(args.output_name, 'w', encoding='ascii') as output:
    output.writelines(create_point_line(args.c, args.b, args.d)
                      for _ in range(args.n))


if __name__ == '__main__':
  main()
