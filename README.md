# Info  
LEX - Legal Elevated eXecution. This is a simple program that gives temporary rights of other user(like sudo or doas, but smaller and simpler)  
  
# Updates  
Added -u flag(run as other user, needs user id, default is root)  
  
# Installation guide  
```bash  
git clone https://github.com/prilter/lex  
cd lex  
su -c "make clean release && cp bin/lex /usr/bin"  
```  
