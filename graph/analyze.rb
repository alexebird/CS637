#!/usr/bin/env ruby

argc = 1
if ARGV.size != argc
  puts "Usage: #{__FILE__} FILE"
  exit
end

fname = ARGV[0]
jobs = {}

data = File.open(fname, 'r').readlines
data.delete_at(-1)
data.each do |e|
  row = e.scan(/\d+/)
  job = row.first
  if !jobs[job]
    jobs[job] = row.last.to_i
  else
    jobs[job] += row.last.to_i
  end
end

max_k, max_v = 0, 0

jobs.each do |k,v|
  max_k = k.size if k.size > max_k
  max_v = v.to_s.size if v.to_s.size > max_v
end

File.open("#{fname}.out", 'w') do |f|
  jobs.each do |k,v|
    f.printf "%*s %*d\n", max_k, k, max_v, v
  end
end

system "cat #{fname}.out"
