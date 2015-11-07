import os

def validate(key):
    lines = key.split("\n")
    name = lines[0]
    if name.upper() != name:
        return False
    digs = map(int, lines[1].split("-"))
    if digs[0] + digs[1] != digs[2]:
        return False
    if ((digs[3] + digs[4]*4) % 12) != digs[5]:
        return False
    if lines[2] != "Marceline Inc":
        return False
    return True

def main():
    if not os.path.isfile("license.key"):
        print "Product not registed. Go away!"
        exit()

    with open("license.key") as license:
        license_key = license.read()
        if validate(license_key):
            print "Awesome, you get to be a vampire! Registered!"
        else:
            print "Your guts get eaten!"

if __name__ == "__main__":
    main()
