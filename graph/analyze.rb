#!/usr/bin/env ruby

# Experiment 1: Find the relative rates of execution for 2 jobs.

argc = 0
if ARGV.size != argc
  puts "Usage: #{__FILE__}"
  exit
end

pids = %w(4 5)
results = []

files = Dir.glob('trial_*.data').sort
3.times { files << files.delete_at(0) }

puts "Input files"
puts "==========="
files.each {|f| printf "%*s\n", files.last.size + 1, f }

jobs = {}

files.each do |fname|
  ratio = fname.scan(/\d+(?=:)/).first.to_i
  jobs.clear

  data = File.open(fname, 'r').readlines.each do |line|
    row = line.scan(/\d+/)
    curr_pid = row.first

    if pids.include? curr_pid  # only keep data about our jobs
      if !jobs[curr_pid]
        jobs[curr_pid] = row.last.to_i
      else
        jobs[curr_pid] += row.last.to_i
      end
    end
  end
  results << [ratio, jobs[pids.first].to_f / jobs[pids.last]]
end

puts "Results"
puts "======="

File.open("results.data", 'w') do |f|
  f.puts "AllocatedRatio ObservedRatio"
  results.each {|e| f.printf "%-14s %-13s\n", e.first, e.last }
end

system "cat results.data"
