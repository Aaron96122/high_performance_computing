#!/usr/bin/julia

# Minimal reference implementation of the cell-distances assignment. It reads
# coordinates from the file "cells" and prints the distance histogram to
# standard output. It does not accept command line arguments; run it as
#
#   ./distances.jl

using OffsetArrays

cells_filename = "cells"
if !isfile(cells_filename)
  error("file $cells_filename must exist")
end
number_points = filesize(cells_filename) ÷ 24

points = zeros(Int16, 3, number_points) :: Matrix{Int16}
open(cells_filename) do file
  for (lx,line) in enumerate(eachline(file))
    points[:,lx] .= [parse(Int16,s[1:3] * s[5:7]) for s in split(line)]
  end
end

counts = OffsetVector(zeros(UInt, 3465+1), -1)

function count_distances(
  points::Matrix{Int16},
  counts::AbstractVector{UInt}
  )
  number_points = size(points,2)
  for px1 in 1:number_points
    for px2 in px1+1:number_points
      @inbounds d1::Float32 = points[1,px1] - points[1,px2]
      @inbounds d2::Float32 = points[2,px1] - points[2,px2]
      @inbounds d3::Float32 = points[3,px1] - points[3,px2]
      dist::Float32 = 1f-1*sqrt(d1*d1 + d2*d2 + d3*d3)
      # Any of the following three variants of converting the floating point
      # distance to an integer is explicitly accepted by the check script.
      @inbounds counts[trunc(Int, dist)]         += 1
      # @inbounds counts[trunc(Int, dist + 0.5f0)] += 1
      # @inbounds counts[round(Int, dist)]         += 1
    end
  end
end
count_distances(points, counts)

for d in eachindex(counts_trunc)
  dd = digits(d; base=10, pad=4)
  lhs = "$(dd[4])$(dd[3]).$(dd[2])$(dd[1])"
  println("$lhs $(counts_trunc[d])")
end
