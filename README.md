# IOT Lâmpada ESP8266: Web Server + Alexa + Google Assistant

## Descrição:
  Esse programa configura a ESP8266 como um dispositivo que recebe comandos para alterar os estados de uma lâmpada comum
  
## Os Arquivos:
    Nota importante: Você deve alterar em todos os arquivos os campos da forma: --INSIRA SEU NOME AQUI--, --INSIRA *ALGO* AQUI--
  - manifest.json
  Esse arquivo torna a página web servida pela placa, um PWA (Progressive Web Application) para Android (basta adicionar a página da web ao Homescreen)
  - index.html
  Esse arquivo é o script da página web servida
  - WebServerQuarto.ino
  Esse é o arquivo que irá configurar a placa ESP8266
  
 ## Instruções:
  - Crie uma conta no site: https://sinric.com/login?returnUrl=%2F
  - Crie um novo dispositivo do tipo 'Light'
  - Para configurar o Google Assistente, siga as instruções no link: https://github.com/kakopappa/sinric/wiki/How-to-use-Google-Home
  - Para configurar a Alexa, instale a Skill: https://www.amazon.com/dp/B078RGYWQQ (Caso não esteja disponível, configure a sua conta Amazon na região EUA)
  - Não esqueça de alterar os campos de Device ID, SSID, Password, IPAddreess, APIKEY, e Porta no arquivo .ino
  - Instale a ArduinoIDE
  - No ArduinoIDE vá em Arquivo-> Preferências, copie e cole http://arduino.esp8266.com/stable/package_esp8266com_index.json no campo "URL's adicionais para gerenciadores de placas" e clique em 'OK'
  - Você deve adicionar a ferramenta para gravar arquivos na memória flash da placa no link: https://github.com/esp8266/arduino-esp8266fs-plugin/releases/tag/0.5.0 (para isso, baixe o arquivo .zip e extraia na pasta 'tools' do diretório de instalação da arduino IDE. Para descobrir o diretório de instalação, vá em Arquivo-> Preferências e olhe o Local do Sketchbook. Caso nesse local não exista um diretório 'tools', crie um)
  - Lembre-se de alterar os campos da forma --INSIRA *ALGO* AQUI-- nos arquivos .html e .json
  - Para gravar os arquivos manifest.json e index.html na memória, após realizar o passo acima, no ArduinoIDE vá em Ferramentas -> ESP8266 Sketch Data Upload (Isso fará o upload de todos os arquivos do diretório 'data' localizado no mesmo diretório do projeto .ino)
  - Conecte o módulo ESP8266 ao PC e carregar o código na placa
  - Alimente o módulo com uma tensão de 5V e ele estará funcionando
  
  ## Futuramente: Tutorial em vídeo
