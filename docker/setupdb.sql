create table mails(ID bigserial primary key,from_address varchar(256), to_address varchar(256), body bytea, date_received timestamp, username varchar(200));

GRANT INSERT ON TABLE mails to maild;
GRANT DELETE ON TABLE mails  to maild;

GRANT CONNECT ON DATABASE maild to maild;

