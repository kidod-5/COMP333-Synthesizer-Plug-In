import torch

# Load your original model
original_model = torch.jit.load("/Users/sam/Documents/Wesleyan/semester_5/software/COMP333-Synthesizer-Plug-In/RAVE_models/vintage.ts")
original_model.eval()

# print(list(original_model.named_children()))  # you should see "_rave"
# print(list(original_model._rave.named_children()))  # you should see encoder/decoder


class RAVEWrapper(torch.jit.ScriptModule):
    def __init__(self, core):
        super().__init__()

        # Extract encoder/decoder
        enc = core._rave.encoder
        dec = core._rave.decoder

        # Register as submodules so TorchScript can see them
        self.encoder = enc
        self.decoder = dec

        self.register_module("encoder", self.encoder)
        self.register_module("decoder", self.decoder)

    @torch.jit.export
    def encode(self, x: torch.Tensor) -> torch.Tensor:
        out = self.encoder(x)
        # out is typically (z, skip)
        if isinstance(out, (tuple, list)):
            z = out[0]
        else:
            z = out
        return z

    @torch.jit.export
    def decode(self, z: torch.Tensor) -> torch.Tensor:
        return self.decoder(z)

    @torch.jit.script_method
    def forward(self, x: torch.Tensor) -> torch.Tensor:
        z = self.encode(x)
        z = z.transpose(1, 2)
        return self.decode(z)


wrapped_model = RAVEWrapper(original_model)
# print(wrapped_model)
wrapped_model.save("/Users/sam/Documents/Wesleyan/semester_5/software/COMP333-Synthesizer-Plug-In/RAVE_models/vintage_wrapped.ts")

print(wrapped_model)
print(wrapped_model.encode)
print(wrapped_model.decode)


print("Saved wrapped model with encoder/decoder exposed!")





# For the musicnet file below

# import torch

# # Load your original model
# original_model = torch.jit.load("/Users/sam/Documents/Wesleyan/semester_5/software/COMP333-Synthesizer-Plug-In/RAVE_models/musicnet.ts")
# original_model.eval()

# # Wrap it into a new ScriptModule exposing encoder/decoder
# class RAVEWrapper(torch.jit.ScriptModule):
#     def __init__(self, model):
#         super().__init__()
#         # Explicitly register encoder/decoder as submodules
#         self.encoder = model.encoder  # adjust based on internal structure
#         self.decoder = model.decoder

#     @torch.jit.script_method
#     def forward(self, x):
#         # Maintain original behavior
#         z = self.encoder(x)
#         z = z.transpose(1, 2)  # channels <-> time
#         return self.decoder(z)

# # Export the wrapper
# wrapped_model = RAVEWrapper(original_model)
# wrapped_model.save("/Users/sam/Documents/Wesleyan/semester_5/software/COMP333-Synthesizer-Plug-In/RAVE_models/musicnet_wrapped.ts")