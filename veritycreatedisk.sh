set -e dd if=/dev/zero of=ext4.img bs=4k count=1024
mkfs.ext4 ext4.img
tune2fs -c0 -i0 ext4.img
mkdir ext4
sudo mount ext4.img ext4/
sudo echo "This is Test1.txt!" > Test1.txt
sudo echo "This is Test2.txt!" > Test2.txt
sudo echo "This is Test3.txt!" > Test2.txt
sudo mkdir TestDirectory1
sudo mkdir TestDirectory2
sudo cd TestDirectory1
sudo echo "This is Test1_1.txt!" > Test1_1.txt
sudo echo "This is Test1_2.txt!" > Test1_2.txt
sudo mkdir TestDirectory1_1
sudo cd TestDirectory1_1
sudo echo "This is Test1_1_1.txt!" > Test1_1_1.txt
sudo echo "This is Test1_1_2.txt!" > Test1_1_2.txt

sudo umount ext4
