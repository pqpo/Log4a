import zlib
import os
import sys

def main(args):
    decompressor = zlib.decompressobj(-zlib.MAX_WBITS)

    in_file = args[0]
    out_file = args[1]
    _buffer = []
    with open(in_file, "rb") as in_fp:
        _buffer = bytearray(os.path.getsize(in_file))
        in_fp.readinto(_buffer)
    with open(out_file, 'w+') as out_fp:
        tmp_buffer = decompressor.decompress(str(_buffer))
        out_fp.write(tmp_buffer)

if __name__ == "__main__":
    main(sys.argv[1:])

