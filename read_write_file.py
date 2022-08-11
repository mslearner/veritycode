import argparse
import shutil
import ctypes
import struct

parser = argparse.ArgumentParser()


def main():
    setup_sphere_structure(0x12345)

# def main():
#     print("Hello World!")

#     parser.add_argument('--offset', dest='offset', type=int, required=True,
#                         help='please enter the offset in the destination')
#     parser.add_argument('--signature-file', nargs='?', dest='signature_file',
#                         default="roothash_signature.signed", type=str, help='please enter the signature file name')
#     parser.add_argument('--image-file', nargs='?', dest='image_file',
#                         default="test_ext4.img", type=str, help='please enter the image file name')
#     parser.add_argument('--operation', nargs='?', dest='operation',
#                         default=1, type=int, help='extract_signature=0, write_signature=1')
#     parser.add_argument('--size', nargs='?', dest='size',
#                         default="512", type=str, help='size')
#     args = parser.parse_args()

#     print(args)

#     # Make a backup of the destination file

#     # print("copying source file to backup", args.destination_file_name+".backup")
#     # shutil.copyfile(args.destination_file_name,
#     #                args.destination_file_name + ".backup")
#     if args.operation == 0:
#         print("\nextracting signature from "+args.image_file +
#               " into "+args.signature_file+"\n")
#         extract_signature(args.offset, int(args.size),
#                           args.image_file, args.signature_file)
#     elif args.operation == 1:
#         print("\ninjecting signature from " +
#               args.signature_file+" into "+args.image_file+"\n")
#         inject_signature(args.offset, int(args.size),
#                          args.image_file, args.signature_file)
#     else:
#         print("invalid operation")


def extract_from_offset(offset, size, source_file, destination_file, inject_extract):
    print("offset="+str(offset)+"size="+str(size))
    with open(destination_file, "wb") as file_dest:
        with open(source_file, "rb") as file_source:

            if (inject_extract == 0):  # extract
                file_dest.seek(int(offset))
            else:
                file_source.seek(int(offset))  # inject

            count = 0
            while count < int(size):
                # Do stuff with byte.
                byte = file_source.read(1)
                file_dest.write(byte)
                print("byte["+hex(int(offset)+count)+"]="+byte.hex())
                count = count + 1


def extract_signature(offset, size, image_file, signature_file):
    extract_from_offset(offset, int(size), image_file, signature_file, 0)


def inject_signature(offset, size, image_file, signature_file):
    extract_from_offset(offset, int(size), image_file, signature_file, 1)

# <version> <dev> <hash_dev><data_block_size> <hash_block_size><num_data_blocks> <hash_start_block><algorithm> <digest> <salt>


# def setup_sphere_structure(hash, hash_size, data_block_size, hash_block_size, hash_start_block, algorithm, digest):
def setup_sphere_structure(hash):
    x = 1000
    bytes_val = hash.to_bytes(64, 'big')
    print(bytes_val)
    var = struct.pack('64s', bytes_val)
    print(var)
    var1 = struct.unpack('64s', var)
    print(hex(var1))


if __name__ == "__main__":
    main()
