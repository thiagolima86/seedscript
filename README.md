# Seeds Cript
You can encript your seeds phrase, to save safe. This program uses AES256 method to hide your keywords. Then you need to have a password and the iv file to decode your hash. You can't lost the password and th iv file, then keep it in a safe place. The password is created by use, but iv file is created by this aplication.


## How to compile

`gcc decode.c -o decode -lcrypto -lssl`
`gcc encode.c -o decode -lcrypto -lssl`
