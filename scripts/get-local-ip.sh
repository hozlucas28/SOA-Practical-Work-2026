#! /bin/bash

# Parse options
options=$(getopt -o "h" --long "help" -- "$@")

if [ $? -ne 0 ]; then
	echo -e "\e[31mAn error occurred on parsing options.\e[0m" >&2
	exit 1
fi

eval set -- "$options"

while true; do
	case "$1" in
		"-h" | "--help")
			need_help="true"
			shift 1
			break
			;;
		"--")
			shift
			break
			;;
		*)
			echo -e "\e[31mAn internal error occurred!\e[0m" >&2
			exit 1
			;;
	esac
done

# Show help if needed
if [[ -n "$need_help" ]]; then
	printf "Usage: $0 [OPTIONS]...



Options:
	-h, --help     display this help and exit
"
	exit 0
fi

os=$(uname -s)

if [[ "$os" == "Darwin" ]]; then
    # macOS
    ifconfig | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | head -n 1
elif [[ "$os" == *"MINGW"* || "$os" == *"MSYS"* || "$os" == *"CYGWIN"* ]]; then
    # Windows
    ipconfig | awk '/IPv4/ {print $NF}' | tr -d '\r' | head -n 1
elif [[ -n "$WSL_DISTRO_NAME" ]]; then
    # Windows (WSL)
    ipconfig.exe | awk '/IPv4/ {print $NF}' | tr -d '\r' | head -n 1
else
    # Linux
    hostname -I | awk '{print $1}'
fi
