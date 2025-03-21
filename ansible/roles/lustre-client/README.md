awscli installation
=========

The role will mount LustreFSx volume

Requirements
------------

No requirements

Role Variables
--------------
| Variable | Description |
|---|---|
|lustre_mount_endpoint|Endpoint of LustreFSx|
|lustre_mount_path|Path for mounting|

Example Playbook
----------------

```yaml
---
- hosts: localhost
  connection: local
  become: true
  tasks:
    - name: 'Import : lustre-client role'
      include_role: 
        name: lustre-client
```

License
-------

BSD

Author Information
------------------

Dmitrii Kondidatov (d.kondidatov@gmail.com)