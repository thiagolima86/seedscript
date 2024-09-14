#!/usr/bin/env ruby

require 'openssl'
require 'fileutils'

def handle_errors
  abort("An error occurred!")
end

def encrypt(plaintext, key, iv)
  cipher = OpenSSL::Cipher.new('AES-256-CBC')
  cipher.encrypt
  cipher.key = key
  cipher.iv = iv

  encrypted = cipher.update(plaintext) + cipher.final
  encrypted
end

STORAGE_DIR = "storage/#{Time.now.strftime("%Y-%m-%d %H:%M:%S") }"
IV_FILE = "#{STORAGE_DIR}/iv.bin"

# Criar o diretório se ele não existir
FileUtils.mkdir_p(STORAGE_DIR)

# Gerar IV randomicamente
iv = OpenSSL::Random.random_bytes(16)  # 16 bytes para AES-256 (128 bits)

# Salvar IV em um arquivo
File.open(IV_FILE, "wb") do |file|
  file.write(iv)
end

# Solicitar que o usuário insira o texto a ser criptografado
puts "Write your seed phrase:"
plaintext = gets.chomp

# Solicitar que o usuário insira a senha
puts "Write password strong:"
password = gets.chomp

# Derivar uma chave de 256 bits usando PBKDF2
salt = '12345678'
iterations = 10_000
key_len = 32
key = OpenSSL::PKCS5.pbkdf2_hmac_sha1(password, salt, iterations, key_len)

# Criptografar o texto
ciphertext = encrypt(plaintext, key, iv)

# Mostrar o texto cifrado em formato hexadecimal
puts "Seed encripted (in hexadecimal):"
puts ciphertext.unpack1('H*')

# Informar que o IV foi salvo
puts "\nThe IV is save in file: '#{IV_FILE}'."
puts "Keep this file safe. You need it to decode your seed"