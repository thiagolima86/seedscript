require 'openssl'

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

# Solicitar que o usuário insira o texto a ser criptografado
puts "Digite o texto a ser criptografado (máximo 128 caracteres):"
plaintext = gets.chomp

# Solicitar que o usuário insira a senha
puts "Digite a senha (qualquer comprimento):"
password = gets.chomp

# Derivar uma chave de 256 bits usando PBKDF2
salt = '12345678'
iterations = 10_000
key_len = 32
key = OpenSSL::PKCS5.pbkdf2_hmac_sha1(password, salt, iterations, key_len)

# Definir o IV fixo
iv = "0123456789012345"

# Criptografar o texto
ciphertext = encrypt(plaintext, key, iv)

# Mostrar o texto cifrado em formato hexadecimal
puts "Texto criptografado (em hexadecimal):"
puts ciphertext.unpack1('H*')