#! /bin/bash

# Parse options
while [[ $# -gt 0 ]]; do
	case "$1" in
		-h | --help)
			need_help='true'
			shift 1
			break
			;;

		--)
			shift 1
			break
			;;

		-*)
			printf "\e[31mAn invalid option was found!\e[0m\n" >&2
			exit 1
			;;

        *)
            break
            ;;
	esac
done

# Show help if needed
if [ -n "$need_help" ]; then
	printf "Usage: $0 [OPTION]...

Setup project tools.

Options:
	-h, --help   display this help and exit
"
	exit 0
fi

# Change from script directory to project root directory
cd $(cd "$(dirname "$0")/.." && pwd)

if [[ $? -ne 0 ]]; then
	printf "\e[31mFailed to change directory to project root.\e[0m\n" >&2
	exit 1
fi

# Exit on any command failure
set -e

# Install packages (formatters, and git hooks manager)
printf "\e[90m\nInstalling project tools (formatters, and git hooks manager)...\e[0m\n\n"

echo "allowBuilds:
  lefthook: true
" > pnpm-workspace.yaml

pnpm install -D \
	oxfmt@^0 \
	lefthook@^2

printf "\e[32m\nProject tools installed successfully.\e[0m\n"

# Pull images
zizmor_image='ghcr.io/zizmorcore/zizmor:1.22.0' # Linter for GitHub Actions workflows.

printf "\e[90m\nPulling docker images project tools...\e[0m\n\n"

docker pull "$zizmor_image"

printf "\e[32m\nDocker images pulled successfully.\e[0m\n"

# Create bin scripts
echo "#! /bin/bash

docker run \\
	--rm \\
	--name zizmor \\
	--volume /workspaces/SOA-Practical-Work-2026/:/workspaces/SOA-Practical-Work-2026/ \\
	--workdir /workspaces/SOA-Practical-Work-2026/ \\
	$zizmor_image \\
	\"\$@\"
" > /usr/local/bin/zizmor

# Allow execution of scripts
chmod +x /usr/local/bin/zizmor
chmod +x /workspaces/SOA-Practical-Work-2026/scripts/*.sh

# Set git configuration and hooks
printf "\e[90m\nSetting up git configuration and hooks...\e[0m\n"

git config --global --add safe.directory /workspaces/SOA-Practical-Work-2026

pnpm exec lefthook install

printf "\e[32m\nGit configuration and hooks set successfully.\e[0m\n"

# Health check
printf "\e[90m\nRunning health check...\e[0m\n\n"

bash scripts/health-check.sh

printf "\e[32m\nAll tools are installed and working correctly.\e[0m\n"

# Final step
printf "\e[33m\nAs a final step, you have to press \`F1\` and select \"Wokwi: Request a new License\" option.\e[0m\n"
