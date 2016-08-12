# Create a certificate

Run the makeCert script with the password as argument:
```
makeCert -p [password]
```

# Adjust the environment variables

Copy ```windowsstore_env.template.bat``` to ```windowsstore_env.user.bat``` and change the defined variables of ```windowsstor_env.user.bat``` to the locations on your computer.

# Compiling and creating the appx packages

To create all necessary packages just run the ```windowsstorePublish.bat``` script with argument -pwd [password], whereby password is the password of the certificate you created in a previous step.
```
windowsstorePublish -pwd [password]
```

# Install as local package (optional)

Import the certificate authority so that the app can be installed on the local computer.
Open a **administrator shell** in the windowsstore dir (where the certificates were created) and run:
```
Certutil -addStore TrustedPeople entropypianotuner_publish_key.cer
```

Start a power shell in ```Entropy-Piano-Tuner/publish``` and run ```Add-AppxPackage entropypianotuner_[x64,x86,...].appx``` to install the desired package locally. E.g. for a x64 machine:
```
Add-AppxPackage entropypianotuner_x64.appx
```

# Upload to windows app store