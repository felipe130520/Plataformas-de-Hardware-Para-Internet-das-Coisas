# Projeto Final da Materia

---

## 📌 Visão Geral do Projeto

Este projeto consiste no desenvolvimento de um sistema IoT para automação do registro de presença de alunos em um setor da UFRN (como o laboratório do Icc), substituindo as tradicionais listas de assinatura em papel.

Ao aproximar o cartão ou *tag* RFID/NFC do leitor, o sistema identifica o aluno, alterna automaticamente o status entre **Entrada** e **Saída**, armazena a data/hora exata sincronizada via NTP e envia os dados via Wi-Fi para a nuvem, atualizando um painel web em tempo real.

---

## 📂 Estrutura de Arquivos

Abaixo está a organização típica dos arquivos presentes nesta pasta:

```text
Roteiros de Introducao e Resumo/
├── README.md                     
├── Proposta de Projeto Final.pdf   # PDF com a proposta do projeto e suas diretrizes
├── Codigo/                         # Código fonte do projeto
└── Imagens/                        # Diagramas e fotos do protótipo
```

---

## 💻 Tecnologias e Bibliotecas

- **Firmware (ESP32):** C/C++ utilizando o ecossistema **Arduino IDE / VS Code (PlatformIO)**.
  - `MFRC522`: Biblioteca para controle do leitor RFID.
  - `NTPClient`: Sincronização de data/hora em tempo real.
  - `FirebaseESP32` / `PubSubClient`: Conectividade e envio de dados para o backend.
- **Backend & Cloud:** Firebase Realtime Database (ou Broker MQTT + Node-RED).
- **Frontend / Interface Web:** HTML5, CSS3, JavaScript (vanilla ou framework).

