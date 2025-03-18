awscli installation
=========

The role will install and configure awscli tool

Requirements
------------

No requirements

Role Variables
--------------
| Variable | Description |
|---|---|
|awscli_download_dir|Path to directory for placing package|
|awscli_package_name|Package name|
|awscli_package_url |Donwloading URL. [Source](https://docs.aws.amazon.com/cli/latest/userguide/getting-started-install.html)|

Example Playbook
----------------

```yaml
- hosts: localhost
  connection: local
  tasks:
    - name: 'Import : awscli role'
      include_role: 
        name: awscli
```

License
-------

BSD

Author Information
------------------

Dmitriy Kondidatov
