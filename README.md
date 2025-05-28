# Info  
FLEX - FLexible Elevated eXecution. This is a simple program that gives temporary rights of other user(like sudo or doas, but smaller)  
  
# Updates  
Added -u flag(run as other user, needs user id, default is root)  
  
# Installation guide  
```bash  
git clone https://github.com/prilter/Flex  
cd Flex  
su -c "make clean release && cp bin/flex /usr/bin"  
```  
