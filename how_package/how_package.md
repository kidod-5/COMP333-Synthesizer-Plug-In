# How to create a package for easy installation

We already have the Installer directory created

Note how the dierctory matches how it works in the system directory.

1. Copy the VST3 and AU file into the correct sub-directory:
```
cp -R "MLPlugIn/build/MLPlugIn_artefacts/Debug/AU/MLPlugIn.component" \
      Installer/Payload/Library/Audio/Plug-Ins/Components/

```
```
cp -R "MLPlugIn/build/MLPlugIn_artefacts/Debug/VST3/MLPlugIn.vst3" \
      Installer/Payload/Library/Audio/Plug-Ins/VST3/
```

2. Run the package builder:
```
pkgbuild \
  --root Installer/Payload \
  --scripts Installer \
  --identifier com.dynamicsounds.mlplugin \
  --version 1.0.0 \
  "MLPlugIn-Installer.pkg"
```
Expected output:
```
pkgbuild: Inferring bundle components from contents of Installer/Payload
pkgbuild: Adding component at Library/Audio/Plug-Ins/VST3/MLPlugIn.vst3
pkgbuild: Adding top-level postinstall script
pkgbuild: Wrote package to MLPlugIn-Installer.pkg
```

You will see the VST3 and the AU file in the correct directories:

/Library/Audio/Plug-Ins/VST3/MLPlugin.vst3
/Library/Audio/Plug-Ins/Components/MLPlugIn.component


Finally, ZIP the the ```.pkg``` file (for GitHub)
