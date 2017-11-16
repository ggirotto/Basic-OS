# t2-sistemas-operacionais
Trabalho 2 da disciplina de Sistemas Operacionais de 2017/II do curso de Ciência da Computação da PUCRS. 

Sistema de arquivos virtual FAT de 16 bits 

- Entrada: 2 bytes (16 bits)
- Setor: 512 bytes
- Cluster: 1024 bytes (2 setores)
- Arquivo fat.part de 4MB (4096 clusters) 

Estrutura do sistema de arquivos

- Boot Block: 1 cluster preenchido com 0xbb
- FAT: 8 clusters (8192 bytes) = 4096 clusters * 2 bytes (entrada de 16 bits)
- Root: 1 cluster
- Data Clusters: 4086 clusters

Estrutura estrutura das entradas de diretório:

- Nome do arquivo: 18 bytes
- Atributo do arquivo: 1 byte (valor 0 para arquivo e 1 para diretório)
- Reservado: 7 bytes
- Número do primeiro cluster ocupado: 2 bytes
- Tamanho do arquivo: 4 bytes
