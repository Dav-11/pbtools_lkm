# python codegen

T
```python
def generate_files(infiles,
                   import_paths=None,
                   output_directory='.',
                   options=None):
    """Generate C source code from proto-file(s).

    """

    if options is None:
        options = Options()

    os.makedirs(output_directory, exist_ok=True)

    for filename in infiles:
        parsed = parse_file(filename, import_paths)
        basename = os.path.basename(filename)
        name = camel_to_snake_case(os.path.splitext(basename)[0])

        filename_h = f'{name}.h'
        filename_c = f'{name}.c'

        header, source = generate(name, parsed, filename_h, options)
        filename_h = os.path.join(output_directory, filename_h)
        filename_c = os.path.join(output_directory, filename_c)

        with open(filename_h, 'w') as fout:
            fout.write(header)

        with open(filename_c, 'w') as fout:
            fout.write(source)
```

In particular this part:
```python
parsed = parse_file(filename, import_paths)
basename = os.path.basename(filename)
name = camel_to_snake_case(os.path.splitext(basename)[0])

filename_h = f'{name}.h'
filename_c = f'{name}.c'

header, source = generate(name, parsed, filename_h, options)
filename_h = os.path.join(output_directory, filename_h)
filename_c = os.path.join(output_directory, filename_c)
```

## parse_file
```python
def parse_file(filename, import_paths=None):
    """Parse given proto3-file `filename` and its imports. Returns a
    :class:`~pbtools.parser.Proto` object.

    `import_paths` is a list of paths where to search for imported
    files.

    """

    if import_paths is None:
        import_paths = []

    with open(filename, 'r', encoding='utf-8') as fin:
        return Proto(
            Parser().parse(ignore_comments(fin.read())),
            find_file(filename, import_paths),
            import_paths
        )
```

### Proto costructor
```python
def __init__(self, tree, abspath, import_paths):
    self.abspath = abspath
    self._package = load_package(tree)
    self._imports = load_imports(tree, import_paths)
    namespace = self.namespace_base()
    self._options = load_options(tree)
    self._messages = load_messages(tree, namespace)
    self._services = load_services(tree)
    self._enums = load_enums(tree, namespace)
    self.messages_stack = []
    self.resolve_messages_fields_types()
    self.resolve_messages_fields_type_kinds()
    self._messages = self.sort_messages_by_usage(self._messages)
```

### Parser().parse()
```python
def parse(self, text, token_tree=False, match_sof=False):

    try:
        tokens = self.tokenize(text)

        if len(tokens) == 0 or tokens[-1].kind != '__EOF__':
            tokens.append(Token('__EOF__', '__EOF__', len(text)))

        if not match_sof:
            if len(tokens) > 0 and tokens[0].kind == '__SOF__':
                del tokens[0]

        return Grammar(self.grammar()).parse(tokens, token_tree)
    except (TokenizeError, GrammarError) as e:
        raise ParseError(text, e.offset)
```

## generate
```python
def generate(namespace, parsed, header_name, options):
    return Generator(namespace, parsed, header_name, options).generate()
```

### Generator().generate()
```python
def generate(self):
    header = HEADER_FMT.format(
        include_guard=self.generate_include_guard(),
        includes=self.generate_includes(),
        types=self.generate_types(),
        declarations=self.generate_declarations(),
        internal_declarations=self.generate_internal_declarations()
    )
    
    source = SOURCE_FMT.format(
        header=self.header_name,
        definitions=self.generate_definitions()
    )

    return header, source
```

