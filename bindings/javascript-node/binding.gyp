{
    "targets": [
   	{
        "target_name": "ingescape",
        "sources": [ "./main.c", "./src/global.c", "./src/admin_config_utils.c",
        "./src/callback_igs.c", "./src/init_control.c", "./src/definition.c", 
        "./src/enumIgs.c", "./src/iop.c", "./src/mapping.c", 
        "./src/licenses.c", "./src/call.c"],
        "include_dirs": [
            '~/builds/sysroot/usr/local/include/',
        ],
        "libraries": [ 
            "~/builds/sysroot/usr/local/lib/libingescape.a", 
            "~/builds/sysroot/usr/local/lib/libczmq.a", 
            "~/builds/sysroot/usr/local/lib/libsodium.a",
            "~/builds/sysroot/usr/local/lib/libzyre.a",
            "~/builds/sysroot/usr/local/lib/libzmq.a"
        ]
    }
    ]
}
