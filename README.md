# Prerequisites
This project needs the following:
- Docker (can be installed with Docker Desktop, check the official documentation)

You can also install WSL 2 on your machine to make Docker run on WSL instead of HyperV. Usage example below was made with WSL in mind.

# Usage example
```sh
# while cd'ed into parent directory of the project, execute on Linux or WSL
cd util && bash build.sh && bash run.sh "example.yaml"
```