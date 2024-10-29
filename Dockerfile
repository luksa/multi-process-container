# Stage 1: Build stage
FROM gcc:latest as builder

# Set the working directory
WORKDIR /

# Copy the source code into the container
COPY memory-consumer.cpp .
COPY http-server.cpp .
COPY cpu-burner.cpp .

# Install required packages for building the application
RUN apt-get update && \
    apt-get install -y build-essential && \
    apt-get clean

# Compile the program with static linking
RUN g++ -static -omemory-consumer memory-consumer.cpp
RUN g++ -static -ohttp-server http-server.cpp
RUN g++ -static -ocpu-burner cpu-burner.cpp

# Stage 2: Final stage
FROM alpine:latest

# Install the necessary libraries to run the static binary
RUN apk add --no-cache libc6-compat curl

# Set the working directory
WORKDIR /

# Copy the compiled binary from the builder stage
COPY --from=builder /memory-consumer /
COPY --from=builder /http-server /
COPY --from=builder /cpu-burner /
COPY entrypoint.sh /
COPY probe.sh /

# Expose the port on which the server will run
EXPOSE 8080

# Command to run the executable
ENTRYPOINT ["/entrypoint.sh"]
