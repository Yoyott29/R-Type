# Contributing to R-Type

Thank you for your interest in contributing. Bug reports, documentation
improvements, code changes, and gameplay ideas are welcome.

## Before you start

1. Read the [README](../README.md) and the
   [developer documentation](https://r-type-project-documentation.vercel.app/developer/contributing).
2. Search existing issues and pull requests before opening a new one.
3. For security vulnerabilities, follow the
   [security policy](SECURITY.md) instead of opening a public issue.

## Development setup

The project requires a C++17 compiler, CMake 3.28 or newer, and Git. Follow
the platform-specific setup and build instructions in the
[README](../README.md):

```sh
cmake -B build
cmake --build build
```

Run the Linux tests before submitting a change:

```sh
./tests/tests.sh
```

## Making changes

- Keep changes focused and consistent with the existing architecture and
  coding style.
- Update documentation when behavior, commands, or public interfaces change.
- Preserve compatibility between the client and authoritative server.
- Do not commit generated build files, downloaded dependencies, credentials, or
  personal configuration.
- Add or update tests when fixing a bug or introducing behavior that can be
  tested automatically.

## Pull requests

1. Create a branch from the current default branch.
2. Make small, focused commits with clear messages.
3. Build the project and run the relevant tests locally.
4. Open a pull request using the provided template.
5. Explain the problem, the solution, and any limitations or platform-specific
   testing that remains.

Maintainers may request changes before merging. Be prepared to keep the branch
up to date and respond to review feedback.

## Questions

For general questions, open a discussion or issue with enough context for
others to reproduce or answer it.

