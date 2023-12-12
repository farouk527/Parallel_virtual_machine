# Master/Slave Sort

# generation de programme PVM
# TP algorithmique parallele
# 

# Cible
CIBLE = upper

# do not edit 

all: $(CIBLE)

clean:
	rm -f *.o $(CIBLE)

.c.o: 
	$(CC) -c $(CFLAGS) -DBPWD="\"`pwd`\"" -o $@ $< 

$(CIBLE): point.o upper.o
	$(CC) -o $@ $@.o $< -lpvm3

