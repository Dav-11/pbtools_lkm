from ..parser import SCALAR_VALUE_TYPES
from ..parser import camel_to_snake_case


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
 * This file was generated by pbtools.
 */

#ifndef {include_guard}
#define {include_guard}

#include "pbtools.h"

{types}
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
 * This file was generated by pbtools.
 */

#include <limits.h>
#include "{header}"

#if CHAR_BIT != 8
#    error "Number of bits in a char must be 8."
#endif

{definitions}\
'''

MESSAGE_STRUCT_FMT = '''\
/**
 * Message {message.full_name}.
 */
{repeated_struct}

struct {message.full_name_snake_case}_t {{
    struct pbtools_message_base_t base;
{members}\
}};
'''

MESSAGE_DECLARATION_FMT = '''\
/**
 * Create a new message {message.full_name} in given workspace.
 *
 * @param[in] workspace_p Message workspace.
 * @param[in] size Workspace size.
 *
 * @return Initialized address book, or NULL on failure.
 */
struct {message.full_name_snake_case}_t *
{message.full_name_snake_case}_new(
    void *workspace_p,
    size_t size);

/**
 * Encode message {message.full_name}.
 *
 * @param[in] self_p Message to encode.
 * @param[out] encoded_p Buffer to encode the message into.
 * @param[in] size Encoded buffer size.
 *
 * @return Encoded data length or negative error code.
 */
int {message.full_name_snake_case}_encode(
    struct {message.full_name_snake_case}_t *self_p,
    uint8_t *encoded_p,
    size_t size);

/**
 * Decode message {message.full_name}.
 *
 * @param[in,out] self_p Initialized message to decode into.
 * @param[in] encoded_p Buffer to decode.
 * @param[in] size Size of the encoded message.
 *
 * @return Number of bytes decoded or negative error code.
 */
int {message.full_name_snake_case}_decode(
    struct {message.full_name_snake_case}_t *self_p,
    const uint8_t *encoded_p,
    size_t size);
'''

MESSAGE_STATIC_DECLARATIONS_FMT = '''\
void {message.full_name_snake_case}_init(
    struct {message.full_name_snake_case}_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct {message.full_name_snake_case}_t *next_p);

void {message.full_name_snake_case}_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct {message.full_name_snake_case}_t *self_p);

void {message.full_name_snake_case}_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct {message.full_name_snake_case}_t *self_p);
'''

MESSAGE_STATIC_DEFINITIONS_FMT = '''\
void {name}_init(
    struct {name}_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct {name}_t *next_p)
{{
    self_p->base.heap_p = heap_p;
    self_p->base.next_p = &next_p->base;
{members_init}
}}

void {name}_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct {name}_t *self_p)
{{
{encode_body}\
}}

void {name}_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct {name}_t *self_p)
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
{finalizers}\
}}
'''

ENCODE_MEMBER_FMT = '''\
    pbtools_encoder_write_{field.full_type_snake_case}(\
encoder_p, {field.field_number}, {ref}self_p->{field.name});
'''

ENCODE_STRING_MEMBER_FMT = '''\
    pbtools_encoder_write_string(\
encoder_p, {field.field_number}, {ref}self_p->{field.name}_p);
'''

ENCODE_REPEATED_MEMBER_FMT = '''\
    pbtools_encoder_write_repeated_{field.full_type_snake_case}(\
encoder_p, {field.field_number}, &self_p->{field.name});
'''

ENCODE_SUB_MESSAGE_MEMBER_FMT = '''\
    pbtools_encoder_sub_message_encode(
        encoder_p,
        {field.field_number},
        &self_p->{field.name}.base,
        (pbtools_message_encode_inner_t){field.full_type_snake_case}_encode_inner);
'''

ENCODE_ENUM_FMT = '''\
    pbtools_encoder_write_enum(encoder_p, {field.field_number}, self_p->{field.name});
'''

ENCODE_ONEOF_FMT = '''\
    {name}_encode(encoder_p, &self_p->{field_name});
'''

ENCODE_ONEOF_MEMBER_FMT = '''\
    case {oneof.full_name_snake_case}_choice_{field.name}_e:
        pbtools_encoder_write_{field.full_type_snake_case}(
            encoder_p,
            {field.field_number},
            {ref}self_p->value.{field.name});
        break;
'''

ENCODE_ONEOF_STRING_MEMBER_FMT = '''\
    case {oneof.full_name_snake_case}_choice_{field.name}_e:
        pbtools_encoder_write_string(
            encoder_p,
            {field.field_number},
            {ref}self_p->value.{field.name}_p);
        break;
'''

ENCODE_ONEOF_SUB_MESSAGE_MEMBER_FMT = '''\
    case {oneof.full_name_snake_case}_choice_{field.name}_e:
        pbtools_encoder_sub_message_encode(
            encoder_p,
            {field.field_number},
            &self_p->value.{field.name}.base,
            (pbtools_message_encode_inner_t){field.full_type_snake_case}_encode_inner);
        break;
'''

ENCODE_ONEOF_ENUM_FMT = '''\
    case {oneof.full_name_snake_case}_choice_{field.name}_e:
        pbtools_encoder_write_enum(
            encoder_p,
            {field.field_number},
            self_p->value.{field.name});
        break;
'''

ENCODE_REPEATED_MESSAGE_MEMBER_FMT = '''\
    {field.full_type_snake_case}_encode_repeated_inner(
        encoder_p,
        {field.field_number},
        &self_p->{field.name});
'''

DECODE_MEMBER_FMT = '''\
        case {field_number}:
            self_p->{field_name} = pbtools_decoder_read_{type}(decoder_p, wire_type);
            break;
'''

DECODE_MEMBER_BYTES_FMT = '''\
        case {field_number}:
            pbtools_decoder_read_bytes(decoder_p, wire_type, &self_p->{field_name});
            break;
'''

DECODE_MEMBER_STRING_FMT = '''\
        case {field_number}:
            pbtools_decoder_read_string(decoder_p, wire_type, &self_p->{field_name}_p);
            break;
'''

DECODE_REPEATED_MEMBER_FMT = '''\
        case {field_number}:
            pbtools_decoder_read_repeated_{type}(
                decoder_p,
                wire_type,
                &self_p->{field_name});
            break;
'''

DECODE_REPEATED_MESSAGE_MEMBER_FMT = '''\
        case {field_number}:
            {type}_decode_repeated_inner(
                decoder_p,
                wire_type,
                &self_p->{field_name});
            break;
'''

DECODE_SUB_MESSAGE_MEMBER_FMT = '''\
        case {field_number}:
            pbtools_decoder_sub_message_decode(
                decoder_p,
                wire_type,
                &self_p->{field_name}.base,
                (pbtools_message_decode_inner_t){type}_decode_inner);
            break;
'''

DECODE_ENUM_FMT = '''\
        case {field_number}:
            self_p->{field_name} = pbtools_decoder_read_enum(decoder_p, wire_type);
            break;
'''

DECODE_ONEOF_FMT = '''\
        case {field_number}:
            {type}_{field_name}_decode(
                decoder_p,
                wire_type,
                self_p);
            break;
'''

DECODE_ONEOF_MEMBER_BYTES_FMT = '''\
    pbtools_decoder_read_bytes(decoder_p,
                               wire_type,
                               &self_p->{oneof.name}.value.{field.name_snake_case});
'''

DECODE_ONEOF_MEMBER_STRING_FMT = '''\
    pbtools_decoder_read_string(decoder_p,
                                wire_type,
                                &self_p->{oneof.name}.value.{field.name_snake_case}_p);
'''

DECODE_ONEOF_MEMBER_FMT = '''\
    self_p->{oneof.name}.value.{field.name_snake_case} = \
pbtools_decoder_read_{field.full_type_snake_case}(
        decoder_p,
        wire_type);
'''

DECODE_ONEOF_SUB_MESSAGE_MEMBER_FMT = '''\
    pbtools_decoder_sub_message_decode(
        decoder_p,
        wire_type,
        &self_p->{oneof.name}.value.{field.name_snake_case}.base,
        (pbtools_message_decode_inner_t){field.full_type_snake_case}_decode_inner);
'''

DECODE_ONEOF_ENUM_FMT = '''\
    self_p->{oneof.name}.value.{field.name_snake_case} = pbtools_decoder_read_enum(
        decoder_p,
        wire_type);
'''

INIT_ONEOF_FIELD_FMT = '''\
void {oneof.full_name_snake_case}_{field.name}_init(
    struct {message.full_name_snake_case}_t *self_p)
{{
    self_p->{oneof.name}.choice = {oneof.full_name_snake_case}_choice_{field.name}_e;
{init}
}}
'''

DECODE_ONEOF_FIELD_FMT = '''\
static void {oneof.full_name_snake_case}_{field.name}_decode(
    struct pbtools_decoder_t *decoder_p,
    int wire_type,
    struct {message.full_name_snake_case}_t *self_p)
{{
    {oneof.full_name_snake_case}_{field.name}_init(self_p);
{decode}\
}}
'''

MESSAGE_DEFINITION_FMT = '''\
struct {message.full_name_snake_case}_t *
{message.full_name_snake_case}_new(
    void *workspace_p,
    size_t size)
{{
    return (pbtools_message_new(
                workspace_p,
                size,
                sizeof(struct {message.full_name_snake_case}_t),
                (pbtools_message_init_t){message.full_name_snake_case}_init));
}}

int {message.full_name_snake_case}_encode(
    struct {message.full_name_snake_case}_t *self_p,
    uint8_t *encoded_p,
    size_t size)
{{
    return (pbtools_message_encode(
                &self_p->base,
                encoded_p,
                size,
                (pbtools_message_encode_inner_t)\
{message.full_name_snake_case}_encode_inner));
}}

int {message.full_name_snake_case}_decode(
    struct {message.full_name_snake_case}_t *self_p,
    const uint8_t *encoded_p,
    size_t size)
{{
    return (pbtools_message_decode(
                &self_p->base,
                encoded_p,
                size,
                (pbtools_message_decode_inner_t)\
{message.full_name_snake_case}_decode_inner));
}}
'''

REPEATED_STRUCT_FMT = '''\
struct {message.full_name_snake_case}_repeated_t {{
    int length;
    struct {message.full_name_snake_case}_t **items_pp;
    struct {message.full_name_snake_case}_t *head_p;
    struct {message.full_name_snake_case}_t *tail_p;
}};\
'''

REPEATED_DECLARATION_FMT = '''\
int {message.full_name_snake_case}_{field.name_snake_case}_alloc(
    struct {message.full_name_snake_case}_t *self_p,
    int length);
'''

REPEATED_DEFINITION_FMT = '''\
int {name}_{field_name}_alloc(
    struct {name}_t *self_p,
    int length)
{{
    return (pbtools_alloc_repeated_{type}(
                &self_p->base,
                length,
                &self_p->{field_name}));
}}
'''

REPEATED_MESSAGE_DEFINITION_ALLOC_FMT = '''\
int {name}_{field_name}_alloc(
    struct {name}_t *self_p,
    int length)
{{
    return (pbtools_alloc_repeated(
                (struct pbtools_repeated_message_t *)&self_p->{field_name},
                length,
                self_p->base.heap_p,
                sizeof(struct {type}_t),
                (pbtools_message_init_t){type}_init));
}}
'''

REPEATED_MESSAGE_DEFINITION_FMT = '''\
void {type}_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct {type}_repeated_t *repeated_p)
{{
    pbtools_encode_repeated_inner(
        encoder_p,
        field_number,
        (struct pbtools_repeated_message_t *)repeated_p,
        (pbtools_message_encode_inner_t){type}_encode_inner);
}}

void {type}_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    int wire_type,
    struct {type}_repeated_t *repeated_p)
{{
    pbtools_decode_repeated_inner(
        decoder_p,
        wire_type,
        (struct pbtools_repeated_message_t *)repeated_p,
        sizeof(struct {type}_t),
        (pbtools_message_init_t){type}_init,
        (pbtools_message_decode_inner_t){type}_decode_inner);
}}

void {type}_finalize_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct {type}_repeated_t *repeated_p)
{{
    pbtools_finalize_repeated_inner(
        decoder_p,
        (struct pbtools_repeated_message_t *)repeated_p);
}}
'''

REPEATED_MESSAGE_STATIC_DECLARATIONS_FMT = '''\
void {message.full_name_snake_case}_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct {message.full_name_snake_case}_repeated_t *repeated_p);

void {message.full_name_snake_case}_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    int wire_type,
    struct {message.full_name_snake_case}_repeated_t *repeated_p);

void {message.full_name_snake_case}_finalize_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct {message.full_name_snake_case}_repeated_t *repeated_p);
'''

REPEATED_FINALIZER_FMT = '''\
    pbtools_decoder_finalize_repeated_{field.full_type_snake_case}(
        decoder_p,
        &self_p->{field.name});\
'''

REPEATED_MESSAGE_FINALIZER_FMT = '''\
    {field.full_type_snake_case}_finalize_repeated_inner(
        decoder_p,
        &self_p->{field.name});\
'''

ENUM_FMT = '''\
/**
 * Enum {full_name}.
 */
enum {name}_e {{
{members}
}};
'''

ENUM_MEMBER_FMT = '''\
    {enum.full_name_snake_case}_{field.name_snake_case}_e = {field.field_number}\
'''

ONEOF_FMT = '''\
/**
 * Oneof {oneof.full_name}.
 */
struct {oneof.full_name_snake_case}_oneof_t {{
    enum {oneof.full_name_snake_case}_choice_e choice;
    union {{
{members}
    }} value;
}};
'''

ONEOF_ENCODE_FMT = '''\
void {oneof.full_name_snake_case}_encode(
    struct pbtools_encoder_t *encoder_p,
    struct {oneof.full_name_snake_case}_oneof_t *self_p)
{{
    switch (self_p->choice) {{

{choices}
    default:
        break;
    }}
}}
'''

ONEOF_INIT_FMT = '''\
void {oneof.full_name_snake_case}_{field.name}_init(
    struct {message.full_name_snake_case}_t *self_p);
'''


class Generator:

    def __init__(self, namespace, parsed, header_name):
        if parsed.package is not None:
            namespace = camel_to_snake_case(parsed.package)

        self.namespace = namespace
        self.parsed = parsed
        self.header_name = header_name

    @property
    def messages(self):
        return self.parsed.messages

    @property
    def package(self):
        return self.parsed.package

    def generate_struct_member_fmt(self, type, name, type_kind):
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
            name = f'{name}_p'
        elif type_kind == 'enum':
            type = f'enum {type}_e '
        elif type_kind == 'message':
            type = f'struct {type}_t '
        else:
            type += ' '

        return f'    {type}{name};'

    def generate_repeated_struct_member_fmt(self, type, name):
        if type in SCALAR_VALUE_TYPES:
            if type in ['sint32', 'sint64']:
                type = type[1:]
            elif type in ['fixed32', 'fixed64']:
                type = f'uint{type[5:]}'
            elif type in ['sfixed32', 'sfixed64']:
                type = f'int{type[6:]}'

            type = f'struct pbtools_repeated_{type}_t'
        else:
            type = f'struct {type}_repeated_t'

        return f'    {type} {name};'

    def generate_struct_members(self, message):
        members = []

        for field in message.fields:
            if field.repeated:
                member = self.generate_repeated_struct_member_fmt(
                    field.full_type_snake_case,
                    field.name)
            else:
                member = self.generate_struct_member_fmt(
                    field.full_type_snake_case,
                    field.name,
                    field.type_kind)

            members.append(member)

        for oneof in message.oneofs:
            type = f'{message.full_name_snake_case}_{oneof.name}_oneof_t'
            members.append(
                f'    struct {type} {oneof.name};')

        members = '\n'.join(members)

        if members:
            members += '\n'

        return members

    def generate_repeated_struct(self, message):
        return REPEATED_STRUCT_FMT.format(message=message)

    def generate_enum_members(self, enum):
        return ',\n'.join([
            ENUM_MEMBER_FMT.format(enum=enum, field=field)
            for field in enum.fields
        ])

    def generate_enum_type(self, enum):
        return [
            ENUM_FMT.format(full_name=enum.full_name,
                            name=enum.full_name_snake_case,
                            members=self.generate_enum_members(enum))
        ]

    def generate_oneof_members(self, oneof):
        members = []

        for field in oneof.fields:
            member = self.generate_struct_member_fmt(
                field.full_type_snake_case,
                field.name,
                field.type_kind)
            members.append(f'    {member}')

        return '\n'.join(members)

    def generate_oneof_choices(self, oneof):
        members = [
            f'    {oneof.full_name_snake_case}_choice_none_e = 0'
        ]

        for i, field in enumerate(oneof.fields, 1):
            members.append(
                f'    {oneof.full_name_snake_case}_choice_{field.name_snake_case}_e = {i}')

        return ENUM_FMT.format(full_name=oneof.full_name,
                               name=f'{oneof.full_name_snake_case}_choice',
                               members=',\n'.join(members))

    def generate_oneof_type(self, oneof):
        types = [self.generate_oneof_choices(oneof)]
        types += [
            ONEOF_FMT.format(oneof=oneof,
                             members=self.generate_oneof_members(oneof))
        ]

        return ['\n'.join(types)]

    def generate_message_types(self, message):
        types = []

        for enum in message.enums:
            types += self.generate_enum_type(enum)

        for inner_message in message.messages:
            types += self.generate_message_types(inner_message)

        for oneof in message.oneofs:
            types += self.generate_oneof_type(oneof)

        types += [
            MESSAGE_STRUCT_FMT.format(
                message=message,
                repeated_struct=self.generate_repeated_struct(message),
                members=self.generate_struct_members(message))
        ]

        return types

    def generate_types(self):
        types = []

        for enum in self.parsed.enums:
            types += self.generate_enum_type(enum)

        for message in self.messages:
            types += self.generate_message_types(message)

        return '\n'.join(types)

    def generate_declarations(self):
        declarations = []
        self.generate_messages_declarations(self.messages,
                                            declarations,
                                            public=True)

        return '\n'.join(declarations)

    def generate_messages_declarations(self, messages, declarations, public):
        for message in messages:
            self.generate_message_declarations(message, declarations, public)

    def generate_message_declarations(self, message, declarations, public):
        for field in message.repeated_fields:
            declarations.append(
                REPEATED_DECLARATION_FMT.format(message=message, field=field))

        for inner_message in message.messages:
            self.generate_message_declarations(inner_message,
                                               declarations,
                                               public=False)

        for oneof in message.oneofs:
            for field in oneof.fields:
                declarations.append(
                    ONEOF_INIT_FMT.format(oneof=oneof,
                                          message=message,
                                          field=field))

        if public:
            declarations.append(
                MESSAGE_DECLARATION_FMT.format(message=message))

    def generate_message_encode_body(self, message):
        members = []
        message_name = message.full_name_snake_case

        for field in reversed(message.fields):
            if field.type_kind == 'scalar-value-type':
                if field.repeated:
                    fmt = ENCODE_REPEATED_MEMBER_FMT
                elif field.type == 'string':
                    fmt = ENCODE_STRING_MEMBER_FMT
                else:
                    fmt = ENCODE_MEMBER_FMT
            else:
                if field.repeated:
                    fmt = ENCODE_REPEATED_MESSAGE_MEMBER_FMT
                elif field.type_kind == 'message':
                    fmt = ENCODE_SUB_MESSAGE_MEMBER_FMT
                else:
                    fmt = ENCODE_ENUM_FMT

            member = fmt.format(field=field,
                                ref='&' if field.type == 'bytes' else '')

            members.append(member)

        for oneof in message.oneofs:
            members.append(
                ENCODE_ONEOF_FMT.format(name=oneof.full_name_snake_case,
                                        field_name=oneof.name))

        return ''.join(members)

    def generate_message_decode_body(self, message):
        members = []

        for field in message.fields:
            if field.repeated:
                if field.type_kind == 'scalar-value-type':
                    fmt = DECODE_REPEATED_MEMBER_FMT
                else:
                    fmt = DECODE_REPEATED_MESSAGE_MEMBER_FMT
            elif field.type == 'bytes':
                fmt = DECODE_MEMBER_BYTES_FMT
            elif field.type == 'string':
                fmt = DECODE_MEMBER_STRING_FMT
            elif field.type_kind == 'scalar-value-type':
                fmt = DECODE_MEMBER_FMT
            elif field.type_kind == 'message':
                fmt = DECODE_SUB_MESSAGE_MEMBER_FMT
            else:
                fmt = DECODE_ENUM_FMT

            members.append(
                fmt.format(type=field.full_type_snake_case,
                           field_name=field.name_snake_case,
                           field_number=field.field_number))

        for oneof in message.oneofs:
            for field in oneof.fields:
                members.append(
                    DECODE_ONEOF_FMT.format(name=oneof.name,
                                            type=oneof.full_name_snake_case,
                                            field_name=field.name,
                                            field_number=field.field_number))

        return '\n'.join(members)

    def generate_message_members_init(self, message):
        members = []

        for field in message.fields:
            name = field.name

            if field.repeated:
                member = f'    self_p->{name}.length = 0;'
            elif field.type == 'bytes':
                member = f'    pbtools_bytes_init(&self_p->{name});'
            elif field.type == 'string':
                member = f'    self_p->{name}_p = "";'
            elif field.type_kind == 'scalar-value-type':
                member = f'    self_p->{name} = 0;'
            elif field.type_kind == 'message':
                member = (f'    {field.full_type_snake_case}_init(&self_p->{name}, '
                          f'heap_p, NULL);')
            else:
                member = f'    self_p->{name} = 0;'

            members.append(member)

        for oneof in message.oneofs:
            members.append(f'    self_p->{oneof.name}.choice = 0;')

        return '\n'.join(members)

    def generate_repeated_definitions(self, message):
        members = []

        for field in message.repeated_fields:
            if field.type_kind == 'scalar-value-type':
                fmt = REPEATED_DEFINITION_FMT
            else:
                fmt = REPEATED_MESSAGE_DEFINITION_ALLOC_FMT

            members.append(fmt.format(name=message.full_name_snake_case,
                                      field_name=field.name,
                                      type=field.full_type_snake_case))

        members.append(
            REPEATED_MESSAGE_DEFINITION_FMT.format(
                type=message.full_name_snake_case))

        return '\n'.join(members)

    def generate_repeated_finalizers(self, message):
        finalizers = []

        for field in message.repeated_fields:
            if field.type_kind == 'scalar-value-type':
                fmt = REPEATED_FINALIZER_FMT
            else:
                fmt = REPEATED_MESSAGE_FINALIZER_FMT

            finalizers.append(fmt.format(field=field))

        if finalizers:
            finalizers = [''] + finalizers + ['']

        return '\n'.join(finalizers)

    def generate_definitions(self):
        declarations = []
        definitions = []

        for message in self.messages:
            self.generate_message_definitions(message,
                                              declarations,
                                              definitions,
                                              public=True)

        return '\n'.join(declarations + definitions)

    def generate_oneof_init_definitions(self,
                                        message,
                                        oneof,
                                        definitions):
        for field in oneof.fields:
            name = field.name

            if field.type == 'bytes':
                init = f'    pbtools_bytes_init(&self_p->{oneof.name}.value.{name});'
            elif field.type == 'string':
                init = f'    self_p->{oneof.name}.value.{name}_p = "";'
            elif field.type_kind == 'scalar-value-type':
                init = f'    self_p->{oneof.name}.value.{name} = 0;'
            elif field.type_kind == 'message':
                init = (f'    {field.full_type_snake_case}_init(\n'
                        f'        &self_p->{oneof.name}.value.{name},\n'
                        f'        self_p->base.heap_p,\n'
                        f'        NULL);')
            else:
                init = f'    self_p->{oneof.name}.value.{name} = 0;'

            definitions.append(
                INIT_ONEOF_FIELD_FMT.format(message=message,
                                            oneof=oneof,
                                            field=field,
                                            init=init))

    def generate_oneof_encode_definitions(self, oneof, definitions):
        choices = []

        for field in oneof.fields:
            if field.type_kind == 'scalar-value-type':
                if field.type == 'string':
                    fmt = ENCODE_ONEOF_STRING_MEMBER_FMT
                else:
                    fmt = ENCODE_ONEOF_MEMBER_FMT
            else:
                if field.type_kind == 'message':
                    fmt = ENCODE_ONEOF_SUB_MESSAGE_MEMBER_FMT
                else:
                    fmt = ENCODE_ONEOF_ENUM_FMT

            choice = fmt.format(oneof=oneof,
                                field=field,
                                ref='&' if field.type == 'bytes' else '')

            choices.append(choice)

        definitions.append(
            ONEOF_ENCODE_FMT.format(oneof=oneof, choices='\n'.join(choices)))

    def generate_oneof_decode_definitions(self,
                                          message,
                                          oneof,
                                          definitions):
        for field in oneof.fields:
            if field.type == 'bytes':
                fmt = DECODE_ONEOF_MEMBER_BYTES_FMT
            elif field.type == 'string':
                fmt = DECODE_ONEOF_MEMBER_STRING_FMT
            elif field.type_kind == 'scalar-value-type':
                fmt = DECODE_ONEOF_MEMBER_FMT
            elif field.type_kind == 'message':
                fmt = DECODE_ONEOF_SUB_MESSAGE_MEMBER_FMT
            else:
                fmt = DECODE_ONEOF_ENUM_FMT

            definitions.append(
                DECODE_ONEOF_FIELD_FMT.format(message=message,
                                              oneof=oneof,
                                              field=field,
                                              decode=fmt.format(oneof=oneof,
                                                                field=field)))

    def generate_oneof_definitions(self,
                                   message,
                                   oneof,
                                   declarations,
                                   definitions):
        self.generate_oneof_init_definitions(message, oneof, definitions)
        self.generate_oneof_encode_definitions(oneof, definitions)
        self.generate_oneof_decode_definitions(message, oneof, definitions)

    def generate_message_definitions(self,
                                     message,
                                     declarations,
                                     definitions,
                                     public):
        declarations.append(
            MESSAGE_STATIC_DECLARATIONS_FMT.format(message=message))

        declarations.append(
            REPEATED_MESSAGE_STATIC_DECLARATIONS_FMT.format(
                message=message))

        for oneof in message.oneofs:
            self.generate_oneof_definitions(message, oneof, declarations, definitions)

        for inner_message in message.messages:
            self.generate_message_definitions(inner_message,
                                              declarations,
                                              definitions,
                                              public=False)

        definitions.append(
            MESSAGE_STATIC_DEFINITIONS_FMT.format(
                name=message.full_name_snake_case,
                encode_body=self.generate_message_encode_body(message),
                decode_body=self.generate_message_decode_body(message),
                members_init=self.generate_message_members_init(message),
                finalizers=self.generate_repeated_finalizers(message)))

        repeated = self.generate_repeated_definitions(message)

        if repeated:
            definitions.append(repeated)

        if public:
            definitions.append(MESSAGE_DEFINITION_FMT.format(message=message))

    def generate(self):
        namespace_upper = self.namespace.upper()
        header = HEADER_FMT.format(include_guard='{}_H'.format(namespace_upper),
                                   types=self.generate_types(),
                                   declarations=self.generate_declarations())
        source = SOURCE_FMT.format(header=self.header_name,
                                   definitions=self.generate_definitions())

        return header, source


def generate(namespace, parsed, header_name):
    """Generate C source code from given parsed proto-file.

    """

    return Generator(namespace, parsed, header_name).generate()
