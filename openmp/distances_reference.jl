#!/usr/bin/julia

# Reference-data generator for the cell-distances assignment. It reads the
# coordinate files in "test_data" and writes histograms there for each accepted
# rounding method and input representation. Run it from this directory as
#
#   ./distances_reference.jl

using OffsetArrays


function write_distances(inputname, outputname_prefix)
  if !isfile(inputname)
    error("file $inputname must exist")
  end
  number_points = filesize(inputname) ÷ 24
  

  points = zeros(Float32, 3, number_points) :: Matrix{Float32}
  open(inputname) do file
    for (lx,line) in enumerate(eachline(file))
      points[:,lx] .= [parse(Float32,s) for s in split(line)]
    end
  end
  
  counts_trunc       = OffsetVector(zeros(UInt, 3465+1), -1)
  counts_trunc_shift = OffsetVector(zeros(UInt, 3465+1), -1)
  counts_round       = OffsetVector(zeros(UInt, 3465+1), -1)

  count_distances(points,
      counts_trunc, counts_trunc_shift, counts_round)
  
  write_outputfile(outputname_prefix * "_trunc", counts_trunc)
  write_outputfile(outputname_prefix * "_trunc_shift", counts_trunc_shift)
  write_outputfile(outputname_prefix * "_round", counts_round)


  points = zeros(Int16, 3, number_points) :: Matrix{Int16}
  open(inputname) do file
    for (lx,line) in enumerate(eachline(file))
      points[:,lx] .= [parse(Int16,s[1:3] * s[5:7]) for s in split(line)]
    end
  end

  counts_trunc       = OffsetVector(zeros(UInt, 3465+1), -1)
  counts_trunc_shift = OffsetVector(zeros(UInt, 3465+1), -1)
  counts_round       = OffsetVector(zeros(UInt, 3465+1), -1)

  count_distances(points,
      counts_trunc, counts_trunc_shift, counts_round)
  
  write_outputfile(outputname_prefix * "_short_trunc", counts_trunc)
  write_outputfile(outputname_prefix * "_short_trunc_shift", counts_trunc_shift)
  write_outputfile(outputname_prefix * "_short_round", counts_round)

end

function count_distances(
  points::Matrix{T},
  counts_trunc::AbstractVector{UInt},
  counts_trunc_shift::AbstractVector{UInt},
  counts_round::AbstractVector{UInt}
  ) where {
  T <: Union{Float32,Int16}
  }
  number_points = size(points,2)
  for px1 in 1:number_points
    for px2 in px1+1:number_points
      @inbounds d1::Float32 = points[1,px1] - points[1,px2]
      @inbounds d2::Float32 = points[2,px1] - points[2,px2]
      @inbounds d3::Float32 = points[3,px1] - points[3,px2]
      dist::Float32 =
        if T === Float32
          1f2*sqrt(d1*d1 + d2*d2 + d3*d3)
        elseif T === Int16
          1f-1*sqrt(d1*d1 + d2*d2 + d3*d3)
        else
          error()
        end
      # Any of the following three variants of converting the floating point
      # distance to an integer is explicitly accepted by the check script.
      @inbounds counts_trunc[trunc(Int, dist)]               += 1
      @inbounds counts_trunc_shift[trunc(Int, dist + 0.5f0)] += 1
      @inbounds counts_round[round(Int, dist)]               += 1
    end
  end
end

function write_outputfile(filename, counts)
  open(filename, "w") do file
    for d in eachindex(counts)
      dd = digits(d; base=10, pad=4)
      lhs = "$(dd[4])$(dd[3]).$(dd[2])$(dd[1])"
      println(file, "$lhs $(counts[d])")
    end
  end
end


write_distances("test_data/cells_1e4", "test_data/dist_1e4")
write_distances("test_data/cells_1e5", "test_data/dist_1e5")
