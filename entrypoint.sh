#!/bin/sh

echo "Running memory-consumer"
./memory-consumer &

# echo "Running CPU burner"
# ./cpu-burner &

echo "Running web server process"
./http-server


