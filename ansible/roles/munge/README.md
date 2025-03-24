Munge
=========

This role will install and configure munge application.

Requirements
------------

Roles: 
- awscli

Validation
----------
```bash
systemctl status munge
munge -n | unmunge

# Expected result
STATUS:          Success (0)
ENCODE_HOST:     hostname (ipv4)
ENCODE_TIME:     2025-03-24 15:23:06 +0000 (1742829786)
DECODE_TIME:     2025-03-24 15:23:06 +0000 (1742829786)
TTL:             300
CIPHER:          aes128 (4)
MAC:             sha256 (5)
ZIP:             none (0)
UID:             ubuntu (1000)
GID:             ubuntu (1000)
LENGTH:          0
```

Role Variables
--------------
|Variable|Description|
|---|---|
|munge_run_state_dir|Contains path to state dir '/run' by default |
|munge_prefix|Contains path to binary installation dir '/usr' by default |
|munge_local_state_dir|Contains path to application state dir '/var' by default |
|munge_sys_config_dir|Contains path to configuration dir '/etc' by default |
|munge_download_url|Contains donwload URL. [Source](https://github.com/dun/munge/releases/latest) |
|munge_package_name|Contains package name with verion |
|munge_package_extension|Contains package extension 'tar.xz' by default |
|munge_download_dir|Contains destination dir for downloaded package '/tmp' by default |
|aws_secret_munge_key|Contains AWS SecretManager's key with munge key stored |
|aws_region|Contains AWS Region. This value is required for secret acquiring |
|munge_dependencies|Contains munge dependency packages|

Example Playbook
----------------

```yaml
---
- hosts: localhost
  connection: local
  tasks:
    - name: 'Import : munge role'
      include_role: 
        name: munge
```

License
-------

BSD

Author Information
------------------

Dmitrii Kondidatov (d.kondidatov@gmail.com)
