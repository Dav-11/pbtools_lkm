FROM python:bookworm AS build-stage

WORKDIR /

RUN apt update && apt install -y plantuml
RUN pip install mkdocs
RUN mkdocs new docs

WORKDIR /docs

# plugin
RUN pip install mkdocs-mermaid2-plugin
RUN pip install mkdocs_puml

# clean
RUN rm -f /docs/mkdocs.yml
RUN rm -rf /docs/docs

# Copy data
COPY ./mkdocs.yml /docs/

RUN mkdir docs
COPY ./docs /docs/docs

# build
RUN mkdocs build

#CMD ["sleep", "infinity"]

FROM nginx:stable-alpine-slim AS production-stage

RUN rm -rf /usr/share/nginx/html
RUN mkdir -p /usr/share/nginx/html

COPY --from=build-stage /docs/site /usr/share/nginx/html
