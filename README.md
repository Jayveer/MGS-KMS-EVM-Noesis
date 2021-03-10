# Metal Gear Solid 2 Noesis Plugin


This is a plugin for [Noesis](https://richwhitehouse.com/index.php?content=inc_projects.php&showproject=91) which allows the user to view textured 3D Models and animations from the game Metal Gear Solid 2: Sons of Liberty.

This project uses Victor Suba's PS2 GS layout swizzling code I would like to thank ForceObscureGaming for making me aware of it. 

![picture](https://github.com/Jayveer/MGS-KMS-EVM-Noesis/blob/master/model.png?raw=true)


### To Do
 - Fix problems with some textures
 - Add other vertex definition flags
##  Usage.

Drag the dll file into the plugins folder of your Noesis folder, run noesis and find and locate the KMS or EVM file you wish to view. Textures will be applied if the associated Tri file can be found. It is best to use [Arsenal](https://github.com/Jayveer/Arsenal) to extract the files so they are in the correct folders and format.

There is a transparency problem with some textures which I will fix, but for now if you see this use the 'cycle blends' option.

There is only one option which when checked allows you to load Mtar animation files for the model if it has bones.

##### Prompt for Motion Archive
This option will allow you to choose an Mar file after the model has loaded. This allows you to view animations provided the bones match.