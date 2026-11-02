# GitHub Actions Linux packages

The existing **Build qView** workflow (`.github/workflows/build.yml`) builds Linux,
macOS, and Windows. Its Ubuntu 22.04 job builds qView once using distribution Qt 5,
then packages the same executable as an AppImage and a deb. There is no separate
Linux packaging workflow.

Push your changes, or use **Actions → Build qView → Run workflow**. The Linux
artifact (`qView[-nightly]-Linux-...`) contains:

- An x86_64 AppImage with bundled Qt libraries and image plugins.
- An amd64 deb using system Qt libraries, with dependencies derived by
  `dpkg-shlibdeps`.
- `SHA256SUMS` for both packages.

Install the deb on Ubuntu 22.04 or a compatible newer distribution:

```sh
sudo apt install ./qview_*.deb
```

Run the AppImage after extracting the downloaded artifact:

```sh
chmod +x qView-*.AppImage
./qView-*.AppImage
```

If FUSE is unavailable, use `./qView-*.AppImage --appimage-extract-and-run`.
Compatibility still depends on the target distribution's system libraries.

Linux packaging does not require custom secrets. The existing Nightly release job
runs after a successful push build on `main`, and uses `GITHUB_TOKEN` with
`contents: write` to create or update the release. Repository or organization
policies may restrict this permission.

The **Checks** workflow remains responsible for unit tests and code checks.
Linux packaging downloads linuxdeploy and its Qt plugin from their upstream
`continuous` releases.
