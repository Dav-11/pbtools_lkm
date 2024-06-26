from ..lkm_common import Options
from ..lkm_common import generate_files


def _do_generate_c_source(args):
    options = Options(args.enums_upper_case)
    generate_files(args.infiles,
                   args.import_path,
                   args.output_directory,
                   options,
                   args.module_type)


def add_subparser(subparsers):
    subparser = subparsers.add_parser(
        'generate_lkm_source',
        description='Generate stub LKM from given protobuf file(s).')
    subparser.add_argument(
        '-I', '--import-path',
        action='append',
        default=[],
        help='Path(s) where to search for imports.')
    subparser.add_argument(
        '-T', '--module-type',
        choices=['netfilter', 'udp_socket'],
        dest='module_type',
        default=1,
        help='Type of LKM to generate. (1 -> Netfilter, 2 -> UDP_SOCKET).')
    subparser.add_argument(
        '-o', '--output-directory',
        default='.',
        help='Output directory (default: %(default)s).')
    subparser.add_argument(
        'infiles',
        nargs='+',
        help='Input protobuf file(s).')
    subparser.add_argument(
        '--enums-upper-case',
        action='store_true',
        help='Use upper case for generated enums.')
    subparser.set_defaults(func=_do_generate_c_source)
