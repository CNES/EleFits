# CFitsIO type mapping table {#type_map}

| Type | Record code | Table code | TFORM | Image code | BITPIX |
| ---- | ----------- | ---------- | ----- | ---------- | ------ |
| bool | TLOGICAL | TBIT | X | TLOGICAL | SBYTE_IMG |
| char | TSBYTE | TSBYTE | S | TSBYTE |  |
| short | TSHORT | TSHORT |  | TSHORT |  |
| int | TINT | TINT |  | TINT |  |
| long | TLONG | TLONG |  | TLONG |  |
| long long | TLONGLONG | TLONGLONG |  | TLONGLONG |  |
| std::int16_t |  |  | I |  | SHORT_IMG |
| std::int32_t |  |  | J |  | LONG_IMG |
| std::int64_t |  |  | K |  | LONGLONG_IMG |
| unsigned char | TBYTE | TBYTE | B | TBYTE | BYTE_IMG |
| unsigned short | TUSHORT | TUSHORT |  | TUSHORT |  |
| unsigned int | TUINT | TUINT |  | TUINT |  |
| unsigned long | TULONG | TULONG |  | TULONG |  |
| unsigned std::int16_t |  |  | U |  | USHORT_IMG |
| unsigned std::int32_t |  |  | V |  | ULONG_IMG |
| unsigned std::int64_t |  |  | W |  |  |
| float | TFLOAT | TFLOAT | E | TFLOAT | FLOAT_IMG |
| double | TDOUBLE | TDOUBLE | D | TDOUBLE | DOUBLE_IMG |
| std::complex<float> | TCOMPLEX | TCOMPLEX | C |  |  |
| std::complex<double> | TDBLCOMPLEX | TDBLCOMPLEX | M |  |  |
| std::string | TSTRING | TSTRING | A |  |  |
