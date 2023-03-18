# Use a Windows base image
FROM rancher/windows_exporter-package:v0.0.3

ENTRYPOINT ["powershell.exe"]

WORKDIR C:\\GraphInputForms

COPY . .

RUN ["C:\\GraphInputForms\\bin\\Test-windows-x86_64\\Tool\\Tool.exe"]