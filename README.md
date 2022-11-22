# secureProgramming
Private message:<br />
The message should be encrypted by using a hash function (probably SHA-256?), the sender should be able to proof that they are legit with their key.

Public message:<br />
The message should also be encrypted by using a hash function (probably SHA-256?), so that mallory can not leak any data of the chat. The sender should be able to proof they are legit with their key.

Register command:<br />
The username and password both should be encrypted by using a hash function (probably SHA-256?).

Login command:<br />
The username and password both should be encrypted by using a hash function (probably SHA-256?).

Key distribution:<br />
Whenever the register command is correctly used, we invoke TTP from our program to link a new user to their public key.

Section 7:<br />
Since we encrypt all messages, mallory can not modify them, on top of that a user should be able to proof that they are legit with their key.
All usernames and passwords, private messages are encrypted, so mallory can not find out any of those.
Mallory can not send messages on behalf of others since each user has a private and public key.

