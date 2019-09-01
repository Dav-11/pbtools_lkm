import re
import time

from .version import __version__


PRIMITIVE_TYPES = [
    'int32',
    'int64',
    'sint32',
    'sint64',
    'uint32',
    'uint64',
    'fixed32',
    'fixed64',
    'sfixed32',
    'sfixed64',
    'float',
    'double',
    'bool',
    'string',
    'bytes'
]

HEADER_FMT = '''\
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by pbtools version {version} {date}.
 */

#ifndef {include_guard}
#define {include_guard}

#include "pbtools.h"

{structs}
{declarations}
#endif
'''

SOURCE_FMT = '''\
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by pbtools version {version} {date}.
 */

#include "{header}"

{definitions}\
'''

MESSAGE_STRUCT_FMT = '''\
/**
 * Message {proto_name} in package {package}.
 */
struct {namespace}_{name}_t {{
    struct pbtools_heap_t *heap_p;
{members}
}};
'''

MESSAGE_DECLARATION_FMT = '''\
/**
 * Create a new message {proto_name} in given workspace.
 *
 * @param[in] workspace_p Message workspace.
 * @param[in] size Workspace size.
 *
 * @return Initialized address book, or NULL on failure.
 */
struct {namespace}_{name}_t *{namespace}_{name}_new(
    void *workspace_p,
    size_t size);

/**
 * Encode message {proto_name} defined in package {package}.
 *
 * @param[in] self_p Message to encode.
 * @param[out] encoded_p Buffer to encode the message into.
 * @param[in] size Encoded buffer size.
 *
 * @return Encoded data length or negative error code.
 */
int {namespace}_{name}_encode(
    struct {namespace}_{name}_t *self_p,
    uint8_t *encoded_p,
    size_t size);

/**
 * Decode message {proto_name} defined in package {package}.
 *
 * @param[in,out] self_p Initialized message to decode into.
 * @param[in] encoded_p Buffer to decode.
 * @param[in] size Size of the encoded message.
 *
 * @return Number of bytes decoded or negative error code.
 */
int {namespace}_{name}_decode(
    struct {namespace}_{name}_t *self_p,
    const uint8_t *encoded_p,
    size_t size);
'''

MESSAGE_INNER_FMT = '''\
static void {namespace}_{name}_encode_inner(
    struct {namespace}_{name}_t *self_p,
    struct pbtools_encoder_t *encoder_p)
{{
{encode_body}\
}}

static void {namespace}_{name}_decode_inner(
    struct {namespace}_{name}_t *self_p,
    struct pbtools_decoder_t *decoder_p)
{{
    int wire_type;

    while (pbtools_decoder_available(decoder_p)) {{
        switch (pbtools_decoder_read_tag(decoder_p, &wire_type)) {{

{decode_body}
        default:
            pbtools_decoder_skip_field(decoder_p, wire_type);
            break;
        }}
    }}
}}
'''

ENCODE_INNER_MEMBER_FMT = '''\
    pbtools_encoder_write_{type}(encoder_p, {field_number}, self_p->{name});
'''

ENCODE_INNER_MEMBER_STRING_FMT = '''\
    pbtools_encoder_write_string(encoder_p, {field_number}, &self_p->{name});
'''

ENCODE_INNER_MEMBER_BYTES_FMT = '''\
    pbtools_encoder_write_bytes(encoder_p, {field_number}, &self_p->{name});
'''

DECODE_INNER_MEMBER_FMT = '''\
        case {field_number}:
            self_p->{name} = pbtools_decoder_read_{type}(decoder_p, wire_type);
            break;
'''

DECODE_INNER_MEMBER_STRING_FMT = '''\
        case {field_number}:
            pbtools_decoder_read_string(decoder_p,
                                        wire_type,
                                        &self_p->{name});
            break;
'''

DECODE_INNER_MEMBER_BYTES_FMT = '''\
        case {field_number}:
            pbtools_decoder_read_bytes(decoder_p,
                                       wire_type,
                                       &self_p->{name});
            break;
'''

MESSAGE_DEFINITION_FMT = '''\
struct {namespace}_{name}_t *{namespace}_{name}_new(
    void *workspace_p,
    size_t size)
{{
    struct {namespace}_{name}_t *self_p;
    struct pbtools_heap_t *heap_p;

    heap_p = pbtools_heap_new(workspace_p, size);

    if (heap_p == NULL) {{
        return (NULL);
    }}

    self_p = pbtools_heap_alloc(heap_p, sizeof(*self_p));

    if (self_p != NULL) {{
        self_p->heap_p = heap_p;
{members_init}
    }}

    return (self_p);
}}

int {namespace}_{name}_encode(
    struct {namespace}_{name}_t *self_p,
    uint8_t *encoded_p,
    size_t size)
{{
    struct pbtools_encoder_t encoder;

    pbtools_encoder_init(&encoder, encoded_p, size);
    {namespace}_{name}_encode_inner(self_p, &encoder);

    return (pbtools_encoder_get_result(&encoder));
}}

int {namespace}_{name}_decode(
    struct {namespace}_{name}_t *self_p,
    const uint8_t *encoded_p,
    size_t size)
{{
    struct pbtools_decoder_t decoder;

    pbtools_decoder_init(&decoder, encoded_p, size, self_p->heap_p);
    {namespace}_{name}_decode_inner(self_p, &decoder);

    return (pbtools_decoder_get_result(&decoder));
}}
'''


def canonical(value):
    """Replace anything but 'a-z', 'A-Z' and '0-9' with '_'.

    """

    return re.sub(r'[^a-zA-Z0-9]', '_', value)


def camel_to_snake_case(value):
    value = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', value)
    value = re.sub(r'(_+)', '_', value)
    value = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', value).lower()
    value = canonical(value)

    return value


def generate_struct_member_fmt(type, name):
    if type in ['int32', 'int64', 'uint32', 'uint64']:
        return f'    {type}_t {name};'
    elif type in ['sint32', 'sint64']:
        return f'    {type[1:]}_t {name};'
    elif type in ['fixed32', 'fixed64']:
        return f'    uint{type[5:]}_t {name};'
    elif type in ['sfixed32', 'sfixed64']:
        return f'    int{type[6:]}_t {name};'
    elif type in ['float', 'double', 'bool']:
        return f'    {type} {name};'
    elif type in ['string', 'bytes']:
        return f'    struct pbtools_bytes_t {name};'
    else:
        return f'    {camel_to_snake_case(type)}_t {name};'


REPEATED_STRUCT_MEMBER_BYTES_FMT = '''\
    struct {{
        uint8_t *buf_p;
        size_t size;
    }} \
'''


REPEATED_STRUCT_MEMBER_FMT = '''\
    struct {{
        int length;
        {type}*items_p;
    }} {name};\
'''


def generate_repeated_struct_member_fmt(type, name):
    if type in ['int32', 'int64', 'uint32', 'uint64']:
        type = f'{type}_t '
    elif type in ['sint32', 'sint64']:
        type = f'{type[1:]}_t '
    elif type in ['fixed32', 'fixed64']:
        type = f'uint{type[5:]}_t '
    elif type in ['sfixed32', 'sfixed64']:
        type = f'int{type[6:]}_t '
    elif type in ['float', 'double', 'bool']:
        type = f'{type} ;'
    elif type == 'string':
        type = f'char *'
    elif type == 'bytes':
        type = REPEATED_STRUCT_MEMBER_BYTES_FMT.format(name=name)
    else:
        type = f'{camel_to_snake_case(type)}_t '

    return REPEATED_STRUCT_MEMBER_FMT.format(type=type, name=name)


def generate_struct_members(message):
    members = []

    for field in message.fields:
        if field.repeated:
            members.append(generate_repeated_struct_member_fmt(field.type,
                                                               field.name))
        else:
            members.append(generate_struct_member_fmt(field.type,
                                                      field.name))

    return members


def generate_structs(namespace, parsed):
    structs = []

    for message in parsed.messages:
        members = generate_struct_members(message)
        structs.append(
            MESSAGE_STRUCT_FMT.format(namespace=namespace,
                                      package=parsed.package,
                                      proto_name=message.name,
                                      name=camel_to_snake_case(message.name),
                                      members='\n'.join(members)))

    return '\n'.join(structs)


def generate_declarations(namespace, parsed):
    declarations = []

    for message in parsed.messages:
        declarations.append(
            MESSAGE_DECLARATION_FMT.format(namespace=namespace,
                                           package=parsed.package,
                                           proto_name=message.name,
                                           name=camel_to_snake_case(message.name)))

    return '\n'.join(declarations)


def generate_message_encode_body(message):
    members = []

    for field in message.fields:
        if field.type == 'string':
            members.append(
                ENCODE_INNER_MEMBER_STRING_FMT.format(field_number=field.field_number,
                                                      name=field.name))
        elif field.type == 'bytes':
            members.append(
                ENCODE_INNER_MEMBER_BYTES_FMT.format(field_number=field.field_number,
                                                     name=field.name))
        elif field.type in PRIMITIVE_TYPES:
            members.append(
                ENCODE_INNER_MEMBER_FMT.format(type=field.type,
                                               field_number=field.field_number,
                                               name=field.name))

    return ''.join(members)


def generate_message_decode_body(message):
    members = []

    for field in message.fields:
        if field.type == 'string':
            members.append(
                DECODE_INNER_MEMBER_STRING_FMT.format(field_number=field.field_number,
                                                      name=field.name))
        elif field.type == 'bytes':
            members.append(
                DECODE_INNER_MEMBER_BYTES_FMT.format(field_number=field.field_number,
                                                     name=field.name))
        elif field.type in PRIMITIVE_TYPES:
            members.append(
                DECODE_INNER_MEMBER_FMT.format(field_number=field.field_number,
                                               type=field.type,
                                               name=field.name))

    return '\n'.join(members)


def generate_message_members_init(message):
    members = []

    for field in message.fields:
        name = field.name

        if field.type == 'string':
            members.append(f'        self_p->{name}.buf_p = (uint8_t *)"";')
            members.append(f'        self_p->{name}.size = 0;')
        elif field.type == 'bytes':
            members.append(f'        self_p->{name}.size = 0;')
        elif field.type in PRIMITIVE_TYPES:
            members.append(f'        self_p->{name} = 0;')

    return '\n'.join(members)


def generate_definitions(namespace, parsed):
    definitions = []

    for message in parsed.messages:
        encode_body = generate_message_encode_body(message)
        decode_body = generate_message_decode_body(message)
        members_init = generate_message_members_init(message)
        definitions.append(
            MESSAGE_INNER_FMT.format(namespace=namespace,
                                     name=camel_to_snake_case(message.name),
                                     encode_body=encode_body,
                                     decode_body=decode_body))
        definitions.append(
            MESSAGE_DEFINITION_FMT.format(namespace=namespace,
                                          package=parsed.package,
                                          proto_name=message.name,
                                          name=camel_to_snake_case(message.name),
                                          members_init=members_init))

    return '\n'.join(definitions)


def generate(namespace, parsed, header_name):
    """Generate C source code from given parsed proto-file.

    """

    date = time.ctime()
    namespace = camel_to_snake_case(parsed.package)
    namespace_upper = namespace.upper()
    include_guard = '{}_H'.format(namespace_upper)

    structs = generate_structs(namespace, parsed)
    declarations = generate_declarations(namespace, parsed)
    definitions = generate_definitions(namespace, parsed)

    header = HEADER_FMT.format(version=__version__,
                               date=date,
                               namespace=namespace,
                               namespace_upper=namespace.upper(),
                               include_guard=include_guard,
                               structs=structs,
                               declarations=declarations)

    source = SOURCE_FMT.format(version=__version__,
                               date=date,
                               namespace=namespace,
                               namespace_upper=namespace.upper(),
                               header=header_name,
                               definitions=definitions)

    return header, source
