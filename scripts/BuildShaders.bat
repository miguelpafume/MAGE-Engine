cd ..
cd shaders\

for %%f in (*.vert) do (
	glslc %%f -o %%f.spv
)

for %%f in (*.frag) do (
	glslc %%f -o %%f.spv
)

cd ..
cd scripts