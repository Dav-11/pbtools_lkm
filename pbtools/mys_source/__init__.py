import os

from ..parser import camel_to_snake_case
from ..parser import parse_file


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

SOURCE_FMT = '''\
# This file was generated by pbtools.

from pbtools import Decoder
from pbtools import Encoder

{types}
'''

MESSAGE_FMT = '''\
class {message.name}:
{members}

    def clear(self):
{clear}

    def to_bytes(self) -> bytes:
        encoder = Encoder()
        self.to_bytes_inner(encoder)

        return encoder.data()

    def from_bytes(self, data: bytes):
        self.clear()
        self.from_bytes_inner(Decoder(data, 0, i64(len(data))))

    def to_bytes_inner(self, encoder: Encoder):
{to_bytes_inner}

    def from_bytes_inner(self, decoder: Decoder):
        while decoder.available():
            field_number, wire_type = decoder.read_tag()

            match field_number:
{from_bytes_inner}
                case _:
                    decoder.skip_field(wire_type)
'''

ENUM_FMT = '''\
@enum
class {name}:
{members}
'''

ENUM_MEMBER_FMT = '''\
    {field.name} = {field.field_number}\
'''

ONEOF_FMT = '''\
    {oneof.name_snake_case}: {oneof.full_name_snake_case}
{members}\
'''

OPTIONAL_STRUCT_MEMBER_FMT = '''\
    struct {{
        bool is_present;
        {type}{value_name_snake_case};
    }} {name_snake_case};\
'''


class Generator:

    def __init__(self, namespace, parsed):
        if parsed.package is not None:
            namespace = camel_to_snake_case(parsed.package)

        self.namespace = namespace
        self.parsed = parsed

    @property
    def messages(self):
        return self.parsed.messages

    @property
    def package(self):
        return self.parsed.package

    def generate_struct_member_fmt(self, type, name_snake_case, type_kind):
        if type in ['int32', 'int64']:
            type = f'i{type[3:]}'
        elif type in ['uint32', 'uint64']:
            type = f'ui{type[4:]}'
        elif type in ['sint32', 'sint64']:
            type = f'i{type[4:]}'
        elif type in ['fixed32', 'fixed64']:
            type = f'u{type[5:]}'
        elif type in ['sfixed32', 'sfixed64']:
            type = f'i{type[6:]}'
        elif type == 'float':
            type = 'f32'
        elif type == 'double':
            type = 'f64'
        elif type in ['bool', 'bytes', 'string']:
            pass
        elif type_kind == 'enum':
            type = f'{type}'
        elif type_kind == 'message':
            type = f'{type}'

        return f'    {name_snake_case}: {type}'

    def generate_struct_clear_value(self, type, type_kind):
        if type in ['int32', 'int64',
                    'uint32', 'uint64',
                    'fixed32', 'fixed64',
                    'sfixed32', 'sfixed64']:
            value = '0'
        elif type in ['float', 'double']:
            value = '0.0'
        elif type == 'bool':
            value = 'False'
        elif type == 'bytes':
            value = 'b""'
        elif type == 'string':
            value = '""'
        elif type_kind == 'enum':
            value = f'{type}.Todo'
        else:
            value = 'None'

        return value

    def generate_optional_struct_member_fmt(self, type, name_snake_case, type_kind):
        value_name_snake_case = 'value'

        if type in ['int32', 'int64', 'uint32', 'uint64']:
            type = f'{type}_t '
        elif type in ['sint32', 'sint64']:
            type = f'{type[1:]}_t '
        elif type in ['fixed32', 'fixed64']:
            type = f'uint{type[5:]}_t '
        elif type in ['sfixed32', 'sfixed64']:
            type = f'int{type[6:]}_t '
        elif type in ['float', 'double', 'bool']:
            type = f'{type} '
        elif type == 'bytes':
            type = f'struct pbtools_bytes_t '
        elif type == 'string':
            type = f'char *'
            value_name_snake_case = f'{value_name_snake_case}_p'
        elif type_kind == 'enum':
            type = f'enum {type}_e '
        else:
            raise Exception('Only scalar value types may be optional.')

        return OPTIONAL_STRUCT_MEMBER_FMT.format(
            type=type,
            name_snake_case=name_snake_case,
            value_name_snake_case=value_name_snake_case)

    def generate_repeated_struct_member_fmt(self,
                                            type,
                                            name_snake_case):
        return f'    {name_snake_case}: [{type}]'

    def generate_struct_members(self, message):
        members = []

        for field in message.fields:
            if field.repeated:
                member = self.generate_repeated_struct_member_fmt(
                    field.type,
                    field.name_snake_case)
            elif field.optional:
                member = self.generate_optional_struct_member_fmt(
                    field.full_type_snake_case,
                    field.name_snake_case,
                    field.type_kind)
            else:
                member = self.generate_struct_member_fmt(
                    field.type,
                    field.name_snake_case,
                    field.type_kind)

            members.append(member)

        for oneof in message.oneofs:
            members.append(
                ONEOF_FMT.format(oneof=oneof,
                                 members=self.generate_oneof_members(oneof)))

        members = '\n'.join(members)

        return members

    def generate_struct_clear(self, message):
        members = []

        for field in message.fields:
            if field.repeated:
                value = '[]'
            elif field.optional:
                value = 'None'
            else:
                value = self.generate_struct_clear_value(
                    field.type,
                    field.type_kind)

            members.append(f'        self.{field.name_snake_case} = {value}')

        members = '\n'.join(members)

        return members

    def generate_struct_to_bytes_inner(self, message):
        lines = []

        for field in reversed(message.fields):
            if field.repeated:
                lines += self.generate_struct_to_bytes_inner_repeated(field)
            else:
                lines += self.generate_struct_to_bytes_inner_field(field)

        lines = '\n'.join(lines)

        return lines

    def generate_struct_to_bytes_inner_field(self, field):
        if field.type_kind == 'enum':
            lines = [f'        encoder.write_enum({field.field_number}, '
                     f'i64(self.{field.name_snake_case}))']
        else:
            lines = [f'        encoder.write({field.field_number}, '
                     f'self.{field.name_snake_case})']

        return lines

    def generate_struct_to_bytes_inner_repeated(self, field):
        return [
            f'        for item in reversed(self.{field.name_snake_case}):',
            f'            pos = encoder.pos',
            f'            item.to_bytes_inner(encoder)',
            f'            encoder.write_length_delimited({field.field_number}, '
            f'u64(pos - encoder.pos))',
            ''
        ]

    def generate_struct_from_bytes_inner(self, message):
        lines = []

        for field in message.fields:
            lines.append(f'                case {field.field_number}:')

            if field.repeated:
                lines += self.generate_struct_from_bytes_inner_repeated(field)
            else:
                lines += self.generate_struct_from_bytes_inner_field(field)

        lines = '\n'.join(lines)

        return lines

    def generate_struct_from_bytes_inner_field(self, field):
        if field.type_kind == 'enum':
            lines = [
                f'                    self.{field.name_snake_case} = '
                f'PhoneType(i64(decoder.read_i32(wire_type)))'
            ]
        else:
            lines = [
                f'                    self.{field.name_snake_case} = '
                f'decoder.read_string(wire_type)'
            ]

        return lines

    def generate_struct_from_bytes_inner_repeated(self, field):
        return [
            f'        for item in reversed(self.{field.name_snake_case}):',
            f'            pos = encoder.pos',
            f'            item.to_bytes_inner(encoder)',
            f'            encoder.write_length_delimited({field.field_number}, '
            f'u64(pos - encoder.pos))',
            ''
        ]

    def generate_enum_members(self, enum):
        return '\n'.join([
            ENUM_MEMBER_FMT.format(enum=enum, field=field)
            for field in enum.fields
        ])

    def generate_enum_type(self, enum):
        return [
            ENUM_FMT.format(full_name=enum.full_name,
                            name=enum.name,
                            members=self.generate_enum_members(enum))
        ]

    def generate_oneof_members(self, oneof):
        members = []

        for field in oneof.fields:
            member = self.generate_struct_member_fmt(
                field.full_type_snake_case,
                field.name_snake_case,
                field.type_kind)
            members.append(f'    {member}')

        return '\n'.join(members)

    def generate_oneof_choices(self, oneof):
        members = [
            f'    {oneof.full_name_snake_case}_none_e = 0'
        ]

        for i, field in enumerate(oneof.fields, 1):
            members.append(
                f'    {oneof.full_name_snake_case}_{field.name_snake_case}_e = {i}')

        return ENUM_FMT.format(full_name=oneof.full_name,
                               name=f'{oneof.full_name_snake_case}',
                               members=',\n'.join(members))

    def generate_oneof_type(self, oneof):
        types = [self.generate_oneof_choices(oneof)]

        return ['\n'.join(types)]

    def generate_message_types(self, message):
        types = []

        for enum in message.enums:
            types += self.generate_enum_type(enum)

        for sub_message in message.messages:
            types += self.generate_message_types(sub_message)

        for oneof in message.oneofs:
            types += self.generate_oneof_type(oneof)

        types += [
            MESSAGE_FMT.format(
                message=message,
                members=self.generate_struct_members(message),
                clear=self.generate_struct_clear(message),
                to_bytes_inner=self.generate_struct_to_bytes_inner(message),
                from_bytes_inner=self.generate_struct_from_bytes_inner(message))
        ]

        return types

    def generate_types(self):
        types = []

        for enum in self.parsed.enums:
            types += self.generate_enum_type(enum)

        for message in self.messages:
            types += self.generate_message_types(message)

        return '\n'.join(types)

    def generate(self):
        return SOURCE_FMT.format(
            types=self.generate_types())


def generate(namespace, parsed):
    """Generate Mys source code from given parsed proto-file.

    """

    return Generator(namespace, parsed).generate()


def generate_files(infiles,
                   import_paths=None,
                   output_directory='.'):
    """Generate Mys source code from proto-file(s).

    """

    for filename in infiles:
        parsed = parse_file(filename, import_paths)
        basename = os.path.basename(filename)
        name = camel_to_snake_case(os.path.splitext(basename)[0])
        source = generate(name, parsed)

        with open(os.path.join(output_directory, f'{name}.mys'), 'w') as fout:
            fout.write(source)
