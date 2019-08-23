{
    "targets": [
   	{
    "target_name": "ingescape",
    "sources": [ "./main.c", "./src/admin_config_utils.c",
    "./src/callback_igs.c", "./src/global.c", "./src/init_control.c",
    "./src/definition.c", "./src/iop.c", "./src/mapping.c", "./src/licenses.c"],
    "include_dirs": [
	    '/usr/local/include/',
	],
    "libraries": [
        '-lingescape', '-L/usr/local/lib/'
    ]
    }
    ]
}
