# Prerequisites
This project needs the following:
- Docker (can be installed with Docker Desktop, check the [official documentation](https://docs.docker.com/get-docker/))

You can also install WSL 2 on your machine to make Docker [run on WSL instead of HyperV](https://docs.docker.com/desktop/windows/wsl/). Usage example below was made with WSL in mind.

# Usage example
```sh
# while cd'ed into parent directory of the project, execute on Linux or WSL
cd util && bash build.sh && bash run.sh "example.yaml"
```

`util/build.sh` file builds a docker image of this application to compile and to run and it also embeds dependencies like [yaml-cpp](https://github.com/jbeder/yaml-cpp), ensuring cross-platform usability.

`util/run.sh` file takes a single input argument that should be the file names of the yaml files (extension included) which reside in `config` directory. It runs the pre-built image of this application with given file name input.

**NOTE:** Before running those scripts, please change the current directory to `util` directory with `cd util` command.
