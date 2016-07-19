create user maild with encrypted password 'maild';
create database maild;

\c maild

create table mails(ID bigserial primary key,from_address varchar(256), to_address varchar(256), body text, date_received timestamp);
alter table mails add column username varchar(200);

GRANT INSERT ON TABLE mails to maild;
GRANT DELETE ON TABLE mails  to maild;

#GRANT SELECT ON ALL SEQUENCES IN SCHEMA  maild to maild;
grant usage,select on sequence mails_id_seq to maild_smtp;

GRANT CONNECT ON DATABASE maild to maild;

