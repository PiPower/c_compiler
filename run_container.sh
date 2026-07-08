docker run -v ./:/home/ubuntu/src/c_compiler --workdir /home/ubuntu/src/c_compiler \
 simple_env sh -c "chmod +x scripts/run_tests.sh && ./scripts/run_tests.sh"