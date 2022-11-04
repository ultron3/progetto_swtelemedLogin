import jwt
encoded_jwt = jwt.encode({"some": "payload"}, "secret", algorithm="HS256") 
print(encoded_jwt)

jwt.decode(encoded_jwt, "secret", algorithms=["HS256"])
{'some': 'payload'}

payload_data = {
    "sub": "4242",
    "name": "Jessica Temporal",
    "nickname": "Jess"
}

my_secret = 'my_super_secret'