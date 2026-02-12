# Security Policy

## Reporting Security Issues

**DO NOT** open a public GitHub issue for security vulnerabilities. Instead, please email security@moneytracker.local (or reach out via private channels) with:
- A clear description of the vulnerability
- Steps to reproduce (if applicable)
- Potential impact
- Any proposed fixes

We will respond within 48 hours and work with you on a fix and coordinated disclosure.

## Security Practices

This project follows these security guidelines:

### Input Validation
- All file paths are validated and sanitized
- CSV fields are checked for suspicious patterns and size limits
- User input is validated before use

### File Handling
- Files are opened with minimal required permissions
- Path traversal attacks are prevented
- File sizes are validated before processing

### Data Protection
- Sensitive financial data fields can be redacted in logs
- No credentials are stored in memory longer than necessary
- Configuration files should not contain passwords

### Dependencies
- Boost, spdlog, libxlsxwriter, and GTK3 are regularly updated
- Dependency vulnerabilities are tracked and patched promptly
- Compile-time sanitizers (ASAN, UBSAN) catch memory/logic issues

### Build Security
- Code is compiled with:
  - `-Wall -Wextra` (strict warnings)
  - AddressSanitizer and UndefinedBehaviorSanitizer (in CI)
- clang-tidy and static analysis run in CI
- Code review and testing required before merge

### Deployment
- Use release distribution packages (AppImage, Debian)
- Verify SHA256 checksums
- Never run with elevated privileges unless absolutely necessary

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 1.1.x   | ✅ Yes (current)   |
| 1.0.x   | ⚠️ EOL (Jan 2027)  |

## Bug Bounty

This is an open-source project maintained by volunteers. We do not currently offer a formal bug bounty program, but we deeply appreciate responsible disclosure of security issues.
