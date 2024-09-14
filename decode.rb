#!/usr/bin/env ruby

require 'openssl'
require 'fileutils'

def handle_errors
  abort("An error occurred!")
end

def decrypt(ciphertext, key, iv)
  decipher = OpenSSL::Cipher.new('AES-256-CBC')
  decipher.decrypt
  decipher.key = key
  decipher.iv = iv

  decrypted = decipher.update(ciphertext) + decipher.final
  decrypted
end

IV_FILE = "storage/iv.bin"

puts "Put your IV file in #{IV_FILE}"
gets.chomp

# Carregar o IV do arquivo
iv = File.open(IV_FILE, "rb") do |file|
  file.read
end

# Solicitar que o usuário insira o texto cifrado
puts "Write your encrypted seed phrase (in hexadecimal):"
ciphertext_hex = gets.chomp
ciphertext = [ciphertext_hex].pack('H*') # Converter hexadecimal para binário

# Solicitar que o usuário insira a senha
puts "Write your password:"
password = gets.chomp

# Derivar a chave de 256 bits usando PBKDF2
salt = '12345678'
iterations = 10_000
key_len = 32
key = OpenSSL::PKCS5.pbkdf2_hmac_sha1(password, salt, iterations, key_len)


# Descriptografar o texto
begin
  plaintext = decrypt(ciphertext, key, iv)
  puts "Decrypted Seed Phrase:"
  puts "---------------------"
  puts plaintext
  puts "---------------------"
rescue => e
  handle_errors
end