#!/bin/bash
# MoneyTracker Release Script
# Usage: ./scripts/release.sh 1.2.0

set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <version>"
    echo "Example: $0 1.2.0"
    exit 1
fi

VERSION=$1
TAG="v${VERSION}"

# Validate version format
if ! [[ $VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: Version must be in format X.Y.Z (e.g., 1.2.0)"
    exit 1
fi

# Check if tag already exists
if git rev-parse "$TAG" >/dev/null 2>&1; then
    echo "Error: Tag $TAG already exists"
    exit 1
fi

# Ensure we're on main/master branch
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [[ "$CURRENT_BRANCH" != "main" && "$CURRENT_BRANCH" != "master" ]]; then
    echo "Error: Must be on main or master branch, currently on $CURRENT_BRANCH"
    exit 1
fi

# Check for uncommitted changes
if ! git diff-index --quiet HEAD --; then
    echo "Error: Uncommitted changes detected"
    exit 1
fi

# Update version in CMakeLists.txt
sed -i "s/VERSION [0-9]*\.[0-9]*\.[0-9]*/VERSION ${VERSION}/" CMakeLists.txt || true

# Commit version bump
git add CMakeLists.txt
git commit -m "Bump version to ${VERSION}" || true

# Create annotated tag
git tag -a "$TAG" -m "Release MoneyTracker ${VERSION}"

echo "Release tag $TAG created successfully!"
echo ""
echo "Next steps:"
echo "  1. Review changes: git log --oneline -n 5"
echo "  2. Push to GitHub: git push origin $TAG"
echo "  3. GitHub Actions will automatically build and create release artifacts"
echo ""
echo "Track the release: https://github.com/YOU/MoneyTracker/releases/tag/$TAG"
