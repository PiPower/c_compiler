docker run -v ./:/home/ubuntu/src/c_compiler --workdir /home/ubuntu/src/c_compiler --rm \
 simple_env sh -c "chmod +x examples/run_tests.sh && examples/run_tests.sh"